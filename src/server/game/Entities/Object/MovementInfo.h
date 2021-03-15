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

#ifndef MovementStatus_h__
#define MovementStatus_h__

#include "G3D/Vector2.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Position.h"

enum MovementFlags : uint32;
enum MovementFlags2 : uint32;

struct MovementTransportData
{
    Position Pos;
    ObjectGuid Guid;
    uint8 VehicleSeatIndex = 0;
    uint32 MoveTime = 0;
    Optional<uint32> PrevMoveTime;
    Optional<int32> VehicleRecID;
};

struct MovementFallVelocity
{
    G3D::Vector2 Direction;
    float Speed = 0.0f;
};

struct MovementFallOrLandData
{
    uint32 Time = 0;
    float JumpVelocity = 0.0f;
    Optional<MovementFallVelocity> Velocity;
};

struct MovementStatus
{
    Position Pos;
    ObjectGuid MoverGUID;
    uint32 MovementFlags0 = 0;
    uint32 MovementFlags1 = 0;
    uint32 MoveTime = 0;
    uint32 MoveIndex = 0;
    float Pitch = 0.f;
    float StepUpStartElevation = 0.f;
    bool HeightChangeFailed = false;
    bool HasSpline = false;

    Optional<MovementTransportData> Transport;
    Optional<MovementFallOrLandData> Fall;

    // Helpers
    bool HasMovementFlag(MovementFlags flag) const { return (MovementFlags0 & flag) != 0; }
    bool HasMovementFlag(MovementFlags2 flag) const { return (MovementFlags1 & flag) != 0; }
    void RemoveMovementFlag(MovementFlags flag) { MovementFlags0 &= ~flag; }
    void RemoveMovementFlag(MovementFlags2 flag) { MovementFlags1 &= ~flag; }
    void AddMovementFlag(MovementFlags flag) { MovementFlags0 |= flag; };
    void AddMovementFlag(MovementFlags2 flag) { MovementFlags1 |= flag; };
    void SetMovementFlag(MovementFlags flag) { MovementFlags0 = flag; };
    void SetMovementFlag(MovementFlags2 flag) { MovementFlags1 = flag; };
    uint32 GetMovementFlags() const { return MovementFlags0; }
    uint32 GetMovementFlags2() const { return MovementFlags1; }
};

#endif // MovementStatus_h__
