# aipc
AI-IPC
when build camera board,configs as :
SET(BOARD_TYPE "IPC")
when build Integrated machine,config as:
SET(BOARD_TYPE "LCDBOARD")

choose your sensor by set env:
    add_definitions(-DSENSOR0_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)
    add_definitions(-DSENSOR1_TYPE=SONY_IMX327_MIPI_2M_30FPS_12BIT)

select sdk version:
SET(SDK_VER "2v0") or SET(SDK_VER "1v0")


