/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SpellPackets.h"
#include "Spell.h"
#include "SpellInfo.h"

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellHealPrediction const& predict)
{
    data << int32(predict.Points);
    data << uint8(predict.Type);
    if (predict.BeaconGUID.has_value())
        data << predict.BeaconGUID->WriteAsPacked();
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::CreatureImmunities const& immunities)
{
    data << int32(immunities.School);
    data << int32(immunities.Value);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::MissileTrajectoryResult const& missileTrajectory)
{
    data << float(missileTrajectory.Pitch);
    data << int32(missileTrajectory.TravelTime);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellAmmo const& spellAmmo)
{
    data << int32(spellAmmo.DisplayID);
    data << int32(spellAmmo.InventoryType);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::RuneData const& runeData)
{
    data << uint8(runeData.Start);
    data << uint8(runeData.Count);
    if (!runeData.Cooldowns.empty())
        data.append(runeData.Cooldowns.data(), runeData.Cooldowns.size());

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::TargetLocation const& targetLocation)
{
    data << targetLocation.Transport.WriteAsPacked();
    data << targetLocation.Location.PositionXYZStream();
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::ProjectileVisual const& projectileVisual)
{
    data << int32(projectileVisual.Id[0]);
    data << int32(projectileVisual.Id[1]);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellTargetData const& spellTargetData)
{
    data << uint32(spellTargetData.Flags);

    if (spellTargetData.Unit.has_value())
        data << spellTargetData.Unit->WriteAsPacked();

    if (spellTargetData.Item.has_value())
        data << spellTargetData.Item->WriteAsPacked();

    if (spellTargetData.SrcLocation)
        data << *spellTargetData.SrcLocation;

    if (spellTargetData.DstLocation)
        data << *spellTargetData.DstLocation;

    if (spellTargetData.Name.has_value())
        data << *spellTargetData.Name;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellHitInfo const& spellHitInfo)
{
    data << uint8(spellHitInfo.HitTargets.size());
    for (ObjectGuid const& hitTarget : spellHitInfo.HitTargets)
        data << hitTarget;

    data << uint8(spellHitInfo.MissStatus.size());
    for (WorldPackets::Spells::SpellMissStatus const& missTarget : spellHitInfo.MissStatus)
    {
        data << missTarget.MissTarget;
        data << uint8(missTarget.Reason);
        if (missTarget.Reason == SPELL_MISS_REFLECT)
            data << uint8(missTarget.ReflectStatus);
    }
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellCastData const& spellCastData)
{
    data << spellCastData.CasterGUID.WriteAsPacked();
    data << spellCastData.CasterUnit.WriteAsPacked();
    data << uint8(spellCastData.CastID);
    data << uint32(spellCastData.SpellID);
    data << uint32(spellCastData.CastFlags);
    data << uint32(spellCastData.CastFlagsEx);
    data << uint32(spellCastData.CastTime);

    if (spellCastData.HitInfo)
    {
        // Hit and miss target counts are both uint8, that limits us to 255 targets for each
        // sending more than 255 targets crashes the client (since count sent would be wrong)
        // Spells like 40647 (with a huge radius) can easily reach this limit (spell might need
        // target conditions but we still need to limit the number of targets sent and keeping
        // correct count for both hit and miss).
        static std::size_t const PACKET_TARGET_LIMIT = std::numeric_limits<uint8>::max();
        if (spellCastData.HitInfo->HitTargets.size() > PACKET_TARGET_LIMIT)
            spellCastData.HitInfo->HitTargets.resize(PACKET_TARGET_LIMIT);

        if (spellCastData.HitInfo->MissStatus.size() > PACKET_TARGET_LIMIT)
            spellCastData.HitInfo->MissStatus.resize(PACKET_TARGET_LIMIT);

        data << *spellCastData.HitInfo;
    }

    data << spellCastData.Target;

    if (spellCastData.RemainingPower.has_value())
        data << uint32(*spellCastData.RemainingPower);

    if (spellCastData.RemainingRunes)
        data << *spellCastData.RemainingRunes;

    if (spellCastData.MissileTrajectory)
        data << *spellCastData.MissileTrajectory;

    if (spellCastData.Ammo)
        data << *spellCastData.Ammo;

    if (spellCastData.ProjectileVisuals)
        data << *spellCastData.ProjectileVisuals;

    if (spellCastData.DestLocSpellCastIndex.has_value())
        data << uint8(*spellCastData.DestLocSpellCastIndex);

    // Todo: TARGET_FLAG_EXTRA_TARGETS (unused as it seems though)

    if (spellCastData.Immunities)
        data << *spellCastData.Immunities;

    if (spellCastData.Predict)
        data << *spellCastData.Predict;

    return data;
}

WorldPacket const* WorldPackets::Spells::SpellStart::Write()
{
    _worldPacket << Cast;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::SpellGo::Write()
{
    _worldPacket << Cast;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::TargetedHealPrediction const& predict)
{
    data << predict.TargetGUID.WriteAsPacked();
    data << predict.Predict;
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::ChannelStartInterruptImmunities const& immunity)
{
    data << immunity.SchoolImmunities;
    data << immunity.Immunities;
    return data;
}

WorldPacket const* WorldPackets::Spells::ChannelStart::Write()
{
    _worldPacket << CasterGUID.WriteAsPacked();
    _worldPacket << uint32(SpellID);
    _worldPacket << int32(ChannelDuration);

    _worldPacket << uint8(InterruptImmunities.has_value());
    if (InterruptImmunities)
        _worldPacket << *InterruptImmunities;

    _worldPacket << uint8(HealPrediction.has_value());
    if (HealPrediction)
        _worldPacket << *HealPrediction;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::MountResult::Write()
{
    _worldPacket << int32(Result);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::AuraDataInfo const& auraData)
{
    data << int32(auraData.SpellID);
    if (auraData.SpellID <= 0)
        return data;

    data << int16(auraData.Flags);
    data << uint8(auraData.CastLevel);
    data << uint8(auraData.Applications);

    if (auraData.CastUnit)
        data << auraData.CastUnit.value().WriteAsPacked();

    if (auraData.Duration)
        data << int32(*auraData.Duration);

    if (auraData.Remaining)
        data << int32(*auraData.Remaining);

    for (uint8 i = 0; i < 3 /*MAX_SPELL_EFFECTS*/; ++i)
        if (auraData.Points[i].has_value())
            data << int32(*auraData.Points[i]);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::AuraInfo const& aura)
{
    data << uint8(aura.Slot);
    data << aura.AuraData;

    return data;
}

WorldPacket const* WorldPackets::Spells::AuraUpdate::Write()
{
    _worldPacket << UnitGUID.WriteAsPacked();

    for (AuraInfo const& aura : Auras)
        _worldPacket << aura;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::AuraUpdateAll::Write()
{
    _worldPacket << UnitGUID.WriteAsPacked();

    for (AuraInfo const& aura : Auras)
        _worldPacket << aura;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::MissileCancel::Write()
{
    _worldPacket.WriteBit(OwnerGUID[7]);
    _worldPacket.WriteBit(OwnerGUID[2]);
    _worldPacket.WriteBit(OwnerGUID[4]);
    _worldPacket.WriteBit(OwnerGUID[6]);
    _worldPacket.WriteBit(Reverse);
    _worldPacket.WriteBit(OwnerGUID[1]);
    _worldPacket.WriteBit(OwnerGUID[0]);
    _worldPacket.WriteBit(OwnerGUID[3]);
    _worldPacket.WriteBit(OwnerGUID[5]);

    _worldPacket.FlushBits();

    _worldPacket.WriteByteSeq(OwnerGUID[6]);
    _worldPacket.WriteByteSeq(OwnerGUID[1]);
    _worldPacket.WriteByteSeq(OwnerGUID[4]);
    _worldPacket.WriteByteSeq(OwnerGUID[2]);
    _worldPacket.WriteByteSeq(OwnerGUID[5]);
    _worldPacket.WriteByteSeq(OwnerGUID[7]);
    _worldPacket << uint32(SpellID);
    _worldPacket.WriteByteSeq(OwnerGUID[0]);
    _worldPacket.WriteByteSeq(OwnerGUID[3]);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::CategoryCooldown::Write()
{
    _worldPacket.reserve(4 + 8 * CategoryCooldowns.size());

    _worldPacket.WriteBits(CategoryCooldowns.size(), 23);
    _worldPacket.FlushBits();

    for (CategoryCooldownInfo const& cooldown : CategoryCooldowns)
    {
        _worldPacket << uint32(cooldown.Category);
        _worldPacket << int32(cooldown.ModCooldown);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::PlaySpellVisual::Write()
{
    _worldPacket << float(TargetPosition.GetPositionZ());
    _worldPacket << int32(SpellVisualID);
    _worldPacket << uint16(MissReason);
    _worldPacket << float(TargetPosition.GetOrientation());
    _worldPacket << float(TargetPosition.GetPositionX());
    _worldPacket << uint16(ReflectStatus);
    _worldPacket << float(TargetPosition.GetPositionY());

    _worldPacket.WriteBit(Target[1]);
    _worldPacket.WriteBit(Source[3]);
    _worldPacket.WriteBit(Source[0]);
    _worldPacket.WriteBit(Target[2]);
    _worldPacket.WriteBit(Target[5]);
    _worldPacket.WriteBit(Source[2]);
    _worldPacket.WriteBit(Source[4]);
    _worldPacket.WriteBit(Target[6]);

    _worldPacket.WriteBit(SpeedAsTime);

    _worldPacket.WriteBit(Source[6]);
    _worldPacket.WriteBit(Target[7]);
    _worldPacket.WriteBit(Source[5]);
    _worldPacket.WriteBit(Source[1]);
    _worldPacket.WriteBit(Source[7]);
    _worldPacket.WriteBit(Target[3]);
    _worldPacket.WriteBit(Target[4]);
    _worldPacket.FlushBits();

    _worldPacket.WriteByteSeq(Source[7]);
    _worldPacket.WriteByteSeq(Source[4]);
    _worldPacket.WriteByteSeq(Target[7]);
    _worldPacket.WriteByteSeq(Source[1]);
    _worldPacket.WriteByteSeq(Source[3]);
    _worldPacket.WriteByteSeq(Source[0]);
    _worldPacket.WriteByteSeq(Source[6]);
    _worldPacket.WriteByteSeq(Target[0]);
    _worldPacket.WriteByteSeq(Target[4]);
    _worldPacket.WriteByteSeq(Source[5]);
    _worldPacket.WriteByteSeq(Target[1]);
    _worldPacket.WriteByteSeq(Target[5]);
    _worldPacket.WriteByteSeq(Target[6]);
    _worldPacket.WriteByteSeq(Target[2]);
    _worldPacket.WriteByteSeq(Source[2]);
    _worldPacket.WriteByteSeq(Target[3]);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::PlaySpellVisualKit::Write()
{
    _worldPacket << uint32(Duration);
    _worldPacket << int32(KitRecID);
    _worldPacket << int32(KitType);

    _worldPacket.WriteBit(Unit[4]);
    _worldPacket.WriteBit(Unit[7]);
    _worldPacket.WriteBit(Unit[5]);
    _worldPacket.WriteBit(Unit[3]);
    _worldPacket.WriteBit(Unit[1]);
    _worldPacket.WriteBit(Unit[2]);
    _worldPacket.WriteBit(Unit[0]);
    _worldPacket.WriteBit(Unit[6]);

    _worldPacket.WriteByteSeq(Unit[0]);
    _worldPacket.WriteByteSeq(Unit[4]);
    _worldPacket.WriteByteSeq(Unit[1]);
    _worldPacket.WriteByteSeq(Unit[6]);
    _worldPacket.WriteByteSeq(Unit[7]);
    _worldPacket.WriteByteSeq(Unit[2]);
    _worldPacket.WriteByteSeq(Unit[3]);
    _worldPacket.WriteByteSeq(Unit[5]);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::SendKnownSpells::Write()
{
    _worldPacket.reserve(1 + 2  + 6 * KnownSpells.size() + 2 + 14 * SpellHistoryEntries.size());

    _worldPacket << uint8(InitialLogin);
    _worldPacket << uint16(KnownSpells.size());

    for (uint32 SpellID : KnownSpells)
    {
        _worldPacket << uint32(SpellID);
        _worldPacket << int16(0); // Slot (unused)
    }

    _worldPacket << uint16(SpellHistoryEntries.size());
    for (SpellHistoryEntry const& entry : SpellHistoryEntries)
    {
        _worldPacket << uint32(entry.SpellID);
        _worldPacket << uint32(entry.ItemID);
        _worldPacket << uint16(entry.Category);
        _worldPacket << int32(entry.RecoveryTime);
        _worldPacket << int32(entry.CategoryRecoveryTime);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::SendUnlearnSpells::Write()
{
    _worldPacket << uint32(Spells.size());
    for (uint32 spellId : Spells)
        _worldPacket << uint32(spellId);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::UnlearnedSpells::Write()
{
    _worldPacket << uint32(SpellID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::UpdateActionButtons::Write()
{
    _worldPacket.append(ActionButtons.data(), ActionButtons.size());
    _worldPacket << Reason;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellModifierData const& spellModifierData)
{
    data << uint8(spellModifierData.ClassIndex);
    data << float(spellModifierData.ModifierValue);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellModifier const& spellModifier)
{
    data << uint32(spellModifier.ModifierData.size());
    data << uint8(spellModifier.ModIndex);
    for (WorldPackets::Spells::SpellModifierData const& modData : spellModifier.ModifierData)
        data << modData;

    return data;
}

WorldPacket const* WorldPackets::Spells::SetSpellModifier::Write()
{
    _worldPacket << uint32(Modifiers.size());
    for (WorldPackets::Spells::SpellModifier const& spellMod : Modifiers)
        _worldPacket << spellMod;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::ConvertRune::Write()
{
    _worldPacket << uint8(Index);
    _worldPacket << uint8(Rune);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::ResyncRunes::Write()
{
    _worldPacket << uint32(Runes.size());
    for (auto const& rune : Runes)
    {
        _worldPacket << uint8(rune.RuneType);
        _worldPacket << uint8(rune.Cooldown);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::AddRunePower::Write()
{
    _worldPacket << uint32(AddedRunesMask);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::SupercededSpells::Write()
{
    _worldPacket << int32(SpellID);
    _worldPacket << int32(Superceded);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::AuraPointsDepleted::Write()
{
    _worldPacket.WriteBit(Unit[2]);
    _worldPacket.WriteBit(Unit[4]);
    _worldPacket.WriteBit(Unit[1]);
    _worldPacket.WriteBit(Unit[7]);
    _worldPacket.WriteBit(Unit[5]);
    _worldPacket.WriteBit(Unit[0]);
    _worldPacket.WriteBit(Unit[3]);
    _worldPacket.WriteBit(Unit[6]);

    _worldPacket.WriteByteSeq(Unit[5]);
    _worldPacket.WriteByteSeq(Unit[0]);
    _worldPacket << uint8(EffectIndex);
    _worldPacket.WriteByteSeq(Unit[3]);
    _worldPacket.WriteByteSeq(Unit[7]);
    _worldPacket.WriteByteSeq(Unit[4]);
    _worldPacket.WriteByteSeq(Unit[2]);
    _worldPacket << uint8(Slot);
    _worldPacket.WriteByteSeq(Unit[6]);
    _worldPacket.WriteByteSeq(Unit[1]);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::SpellFailure::Write()
{
    _worldPacket << CasterUnit.WriteAsPacked();
    _worldPacket << uint8(CastID);
    _worldPacket << int32(SpellID);
    _worldPacket << uint8(Reason);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::SpellFailedOther::Write()
{
    _worldPacket << CasterUnit.WriteAsPacked();
    _worldPacket << uint8(CastID);
    _worldPacket << int32(SpellID);
    _worldPacket << uint8(Reason);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::ResurrectRequest::Write()
{
    _worldPacket << ResurrectOffererGUID;
    _worldPacket << uint32(Name.length() + 1);
    _worldPacket << Name; // client expects a null-terminated string
    _worldPacket << bool(Sickness);
    _worldPacket << bool(UseTimer);
    _worldPacket << int32(SpellID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Spells::SpellDelayed::Write()
{
    _worldPacket << Caster.WriteAsPacked();
    _worldPacket << int32(ActualDelay);
    return &_worldPacket;
}

ByteBuffer& operator>>(ByteBuffer& data, Optional<WorldPackets::Spells::TargetLocation>& location)
{
    location.emplace();
    data >> location->Transport.ReadAsPacked();
    data >> location->Location.m_positionX;
    data >> location->Location.m_positionY;
    data >> location->Location.m_positionZ;

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::Spells::SpellTargetData& targetData)
{
    data >> targetData.Flags;

    if (targetData.Flags & (TARGET_FLAG_UNIT | TARGET_FLAG_UNIT_MINIPET | TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_CORPSE_ENEMY | TARGET_FLAG_CORPSE_ALLY))
    {
        targetData.Unit.emplace();
        data >> targetData.Unit->ReadAsPacked();
    }

    if (targetData.Flags & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
    {
        targetData.Item.emplace();
        data >> targetData.Item->ReadAsPacked();
    }

    if (targetData.Flags & TARGET_FLAG_SOURCE_LOCATION)
        data >> targetData.SrcLocation;

    if (targetData.Flags & TARGET_FLAG_DEST_LOCATION)
        data >> targetData.DstLocation;

    if (targetData.Flags & TARGET_FLAG_STRING)
    {
        targetData.Name.emplace();
        data >> *targetData.Name;
    }

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, Optional<MovementStatus>& movementInfo)
{
    movementInfo.emplace();
    data >> movementInfo->Pos.m_positionZ;
    data >> movementInfo->Pos.m_positionY;
    data >> movementInfo->Pos.m_positionX;

    if (data.ReadBit())
        movementInfo->Fall.emplace();

    bool hasTime = !data.ReadBit();
    bool hasFacing = !data.ReadBit();

    movementInfo->HasSpline = data.ReadBit();
    movementInfo->HeightChangeFailed = data.ReadBit();

    movementInfo->MoverGUID[6] = data.ReadBit();
    movementInfo->MoverGUID[4] = data.ReadBit();

    bool hasExtraMovementFlags = !data.ReadBit();

    movementInfo->MoverGUID[3] = data.ReadBit();
    movementInfo->MoverGUID[5] = data.ReadBit();

    bool hasSplineElevation = !data.ReadBit();
    bool hasPitch = !data.ReadBit();

    movementInfo->MoverGUID[7] = data.ReadBit();

    if (data.ReadBit())
        movementInfo->Transport.emplace();

    movementInfo->MoverGUID[2] = data.ReadBit();

    bool hasMovementFlags = !data.ReadBit();

    movementInfo->MoverGUID[1] = data.ReadBit();
    movementInfo->MoverGUID[0] = data.ReadBit();

    if (movementInfo->Transport.has_value())
    {
        movementInfo->Transport->Guid[6] = data.ReadBit();
        movementInfo->Transport->Guid[2] = data.ReadBit();
        movementInfo->Transport->Guid[5] = data.ReadBit();

        if (data.ReadBit())
            movementInfo->Transport->PrevMoveTime.emplace();

        movementInfo->Transport->Guid[7] = data.ReadBit();
        movementInfo->Transport->Guid[4] = data.ReadBit();
        if (data.ReadBit())
            movementInfo->Transport->VehicleRecID.emplace();

        movementInfo->Transport->Guid[0] = data.ReadBit();
        movementInfo->Transport->Guid[1] = data.ReadBit();
        movementInfo->Transport->Guid[3] = data.ReadBit();
    }

    if (hasExtraMovementFlags)
        movementInfo->MovementFlags1 = data.ReadBits(12);

    if (hasMovementFlags)
        movementInfo->MovementFlags0 = data.ReadBits(30);

    if (movementInfo->Fall.has_value())
        if (data.ReadBit())
            movementInfo->Fall->Velocity.emplace();

    data.ReadByteSeq(movementInfo->MoverGUID[1]);
    data.ReadByteSeq(movementInfo->MoverGUID[4]);
    data.ReadByteSeq(movementInfo->MoverGUID[7]);
    data.ReadByteSeq(movementInfo->MoverGUID[3]);
    data.ReadByteSeq(movementInfo->MoverGUID[0]);
    data.ReadByteSeq(movementInfo->MoverGUID[2]);
    data.ReadByteSeq(movementInfo->MoverGUID[5]);
    data.ReadByteSeq(movementInfo->MoverGUID[6]);

    if (movementInfo->Transport.has_value())
    {
        data >> movementInfo->Transport->VehicleSeatIndex;
        movementInfo->Transport->Pos.SetOrientation(data.read<float>());
        data >> movementInfo->Transport->MoveTime;

        data.ReadByteSeq(movementInfo->Transport->Guid[6]);
        data.ReadByteSeq(movementInfo->Transport->Guid[5]);

        if (movementInfo->Transport->PrevMoveTime.has_value())
            data >> *movementInfo->Transport->PrevMoveTime;

        data >> movementInfo->Transport->Pos.m_positionX;

        data.ReadByteSeq(movementInfo->Transport->Guid[4]);

        data >> movementInfo->Transport->Pos.m_positionZ;

        data.ReadByteSeq(movementInfo->Transport->Guid[2]);
        data.ReadByteSeq(movementInfo->Transport->Guid[0]);

        if (movementInfo->Transport->VehicleRecID.has_value())
            data >> *movementInfo->Transport->VehicleRecID;

        data.ReadByteSeq(movementInfo->Transport->Guid[1]);
        data.ReadByteSeq(movementInfo->Transport->Guid[3]);

        data >> movementInfo->Transport->Pos.m_positionY;

        data.ReadByteSeq(movementInfo->Transport->Guid[7]);
    }

    if (hasFacing)
        movementInfo->Pos.SetOrientation(data.read<float>());

    if (hasSplineElevation)
        data >> movementInfo->StepUpStartElevation;

    if (movementInfo->Fall.has_value())
    {
        data >> movementInfo->Fall->Time;
        if (movementInfo->Fall->Velocity.has_value())
        {
            data >> movementInfo->Fall->Velocity->Direction.y;
            data >> movementInfo->Fall->Velocity->Direction.x;
            data >> movementInfo->Fall->Velocity->Speed;
        }
        data >> movementInfo->Fall->JumpVelocity;
    }

    if (hasTime)
        data >> movementInfo->MoveTime;

    if (hasPitch)
        data >> movementInfo->Pitch;

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::Spells::MissileTrajectoryRequest& missileTrajectory)
{
    data >> missileTrajectory.Pitch;
    data >> missileTrajectory.Speed;

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::Spells::SpellCastRequest& castRequest)
{
    data >> castRequest.CastID;
    data >> castRequest.SpellID;
    data >> castRequest.Misc;
    data >> castRequest.SendCastFlags;
    data >> castRequest.Target;

    if (castRequest.SendCastFlags & CAST_FLAG_HAS_TRAJECTORY)
    {
        data >> castRequest.MissileTrajectory;
        bool hasMovementData = data.read<bool>();
        if (hasMovementData)
            data >> castRequest.MoveUpdate;
    }

    if (castRequest.SendCastFlags & CAST_FLAG_HAS_WEIGHT)
    {
        uint32 weightCount = data.read<uint32>();
        castRequest.Weight.resize(weightCount);

        for (WorldPackets::Spells::SpellWeight& weight : castRequest.Weight)
        {
            data >> weight.Type;
            data >> weight.ID;
            data >> weight.Quantity;
        }
    }

    return data;
}

void WorldPackets::Spells::CastSpell::Read()
{
    _worldPacket >> Cast;
}

void WorldPackets::Spells::UseItem::Read()
{
    _worldPacket >> PackSlot;
    _worldPacket >> Slot;
    _worldPacket >> Cast.CastID;
    _worldPacket >> Cast.SpellID;
    _worldPacket >> CastItem;
    _worldPacket >> Cast.Misc;
    _worldPacket >> Cast.SendCastFlags;
    _worldPacket >> Cast.Target;

    if (Cast.SendCastFlags & CAST_FLAG_HAS_TRAJECTORY)
    {
        _worldPacket >> Cast.MissileTrajectory;
        bool hasMovementData = _worldPacket.read<bool>();
        if (hasMovementData)
            _worldPacket >> Cast.MoveUpdate;
    }

    if (Cast.SendCastFlags & CAST_FLAG_HAS_WEIGHT)
    {
        uint32 weightCount = _worldPacket.read<uint32>();
        Cast.Weight.resize(weightCount);

        for (WorldPackets::Spells::SpellWeight& weight : Cast.Weight)
        {
            _worldPacket >> weight.Type;
            _worldPacket >> weight.ID;
            _worldPacket >> weight.Quantity;
        }
    }
}
