/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WPAD_STATUS_H
#define WPAD_STATUS_H

#include <wut.h>
#include <padscore/wpad.h>

#ifdef __cplusplus
extern "C" {
#endif


#define WPAD_MAX_IR_DOTS 4


typedef struct WPADbVec2D
{
    int8_t x;
    int8_t y;
} WPADbVec2D;
WUT_CHECK_OFFSET(WPADbVec2D, 0x00, x);
WUT_CHECK_OFFSET(WPADbVec2D, 0x01, y);
WUT_CHECK_SIZE(WPADbVec2D, 0x02);


typedef struct WPADVec3D
{
    int16_t x;
    int16_t y;
    int16_t z;
} WPADVec3D;
WUT_CHECK_OFFSET(WPADVec3D, 0x00, x);
WUT_CHECK_OFFSET(WPADVec3D, 0x02, y);
WUT_CHECK_OFFSET(WPADVec3D, 0x04, z);
WUT_CHECK_SIZE(WPADVec3D, 0x06);


typedef struct WPADIRDot
{
    WPADVec2D pos;
    uint16_t  size;
    uint8_t   id;
} WPADIRDot;
WUT_CHECK_OFFSET(WPADIRDot, 0x00, pos);
WUT_CHECK_OFFSET(WPADIRDot, 0x04, size);
WUT_CHECK_OFFSET(WPADIRDot, 0x06, id);
WUT_CHECK_SIZE(WPADIRDot, 0x08);


typedef struct WPADStatus
{
    uint16_t  buttons;
    WPADVec3D acc;
    WPADIRDot ir[WPAD_MAX_IR_DOTS];
    uint8_t   extensionType;
    int8_t    error;
} WPADStatus;
WUT_CHECK_OFFSET(WPADStatus, 0x00, buttons);
WUT_CHECK_OFFSET(WPADStatus, 0x02, acc);
WUT_CHECK_OFFSET(WPADStatus, 0x08, ir);
WUT_CHECK_OFFSET(WPADStatus, 0x28, extensionType);
WUT_CHECK_OFFSET(WPADStatus, 0x29, error);
WUT_CHECK_SIZE(WPADStatus, 0x2a);


typedef struct WPADExtNunchuk {
    WPADVec3D  acc;
    WPADbVec2D stick;
} WPADExtNunchuk;
WUT_CHECK_OFFSET(WPADExtNunchuk, 0x00, acc);
WUT_CHECK_OFFSET(WPADExtNunchuk, 0x06, stick);
WUT_CHECK_SIZE(WPADExtNunchuk, 0x08);


typedef struct WPADNunchukStatus
{
    WPADStatus     core;
    WPADExtNunchuk ext;
} WPADNunchukStatus;
WUT_CHECK_OFFSET(WPADNunchukStatus, 0x00, core);
WUT_CHECK_OFFSET(WPADNunchukStatus, 0x2a, ext);
WUT_CHECK_SIZE(WPADNunchukStatus, 0x32);


typedef struct WPADExtClassic {
    uint16_t  buttons;
    WPADVec2D leftStick;
    WPADVec2D rightStick;
    uint8_t   leftTrigger;
    uint8_t   rightTrigger;
} WPADExtClassic;
WUT_CHECK_OFFSET(WPADExtClassic, 0x00, buttons);
WUT_CHECK_OFFSET(WPADExtClassic, 0x02, leftStick);
WUT_CHECK_OFFSET(WPADExtClassic, 0x06, rightStick);
WUT_CHECK_OFFSET(WPADExtClassic, 0x0a, leftTrigger);
WUT_CHECK_OFFSET(WPADExtClassic, 0x0b, rightTrigger);
WUT_CHECK_SIZE(WPADExtClassic, 0x0c);


typedef struct WPADClassicStatus
{
    WPADStatus     core;
    WPADExtClassic ext;
} WPADClassicStatus;
WUT_CHECK_OFFSET(WPADClassicStatus, 0x00, core);
WUT_CHECK_OFFSET(WPADClassicStatus, 0x2a, ext);
WUT_CHECK_SIZE(WPADClassicStatus, 0x36);


typedef struct WPADExtPro {
    uint32_t   buttons;
    WPADVec2D  leftStick;
    WPADVec2D  rightStick;
    BOOL       charging;
    BOOL       wired;
} WPADExtPro;
WUT_CHECK_OFFSET(WPADExtPro, 0x00, buttons);
WUT_CHECK_OFFSET(WPADExtPro, 0x04, leftStick);
WUT_CHECK_OFFSET(WPADExtPro, 0x08, rightStick);
WUT_CHECK_OFFSET(WPADExtPro, 0x0c, charging);
WUT_CHECK_OFFSET(WPADExtPro, 0x10, wired);
WUT_CHECK_SIZE(WPADExtPro, 0x14);

typedef struct WPADProStatus
{
    WPADStatus core;
    WPADExtPro ext;
} WPADProStatus;
WUT_CHECK_OFFSET(WPADProStatus, 0x00, core);
WUT_CHECK_OFFSET(WPADProStatus, 0x2c, ext);
WUT_CHECK_SIZE(WPADProStatus, 0x40);


// TODO: add structs for train, balance board controllers



typedef struct WPADMPlusStatus {
    WPADStatus core;
    union {
        WPADExtNunchuk nunchuk;
        WPADExtClassic classic;
    } ext;
    uint8_t status;
    WUT_UNKNOWN_BYTES(1);
    WPADVec3D angles;
} WPADMPlusStatus;
WUT_CHECK_OFFSET(WPADMPlusStatus, 0x00, core);
WUT_CHECK_OFFSET(WPADMPlusStatus, 0x2a, ext);
WUT_CHECK_OFFSET(WPADMPlusStatus, 0x2a, ext.nunchuk);
WUT_CHECK_OFFSET(WPADMPlusStatus, 0x2a, ext.classic);
WUT_CHECK_OFFSET(WPADMPlusStatus, 0x36, status);
WUT_CHECK_OFFSET(WPADMPlusStatus, 0x38, angles);
WUT_CHECK_SIZE(WPADMPlusStatus, 0x3e);


#ifdef __cplusplus
}
#endif

#endif
