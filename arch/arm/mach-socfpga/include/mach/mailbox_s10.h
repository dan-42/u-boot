/*
 * Copyright (C) 2017 Intel Corporation <www.intel.com>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#ifndef _MAILBOX_S10_H_
#define _MAILBOX_S10_H_

/* user define Uboot ID */
#define MBOX_CLIENT_ID_UBOOT	0xB
#define MBOX_ID_UBOOT		0x1

#define MBOX_MAX_CMD_INDEX	2047
#define MBOX_CMD_BUFFER_SIZE	32
#define MBOX_RESP_BUFFER_SIZE	16

#define MBOX_HDR_CMD_LSB	0
#define MBOX_HDR_CMD_MSK	(BIT(11) - 1)
#define MBOX_HDR_I_LSB		11
#define MBOX_HDR_I_MSK		BIT(11)
#define MBOX_HDR_LEN_LSB	12
#define MBOX_HDR_LEN_MSK	0x007FF000
#define MBOX_HDR_ID_LSB		24
#define MBOX_HDR_ID_MSK		0x0F000000
#define MBOX_HDR_CLIENT_LSB	28
#define MBOX_HDR_CLIENT_MSK	0xF0000000

/* Interrupt flags */
#define MBOX_FLAGS_INT_COE	BIT(0)	/* COUT update interrupt enable */
#define MBOX_FLAGS_INT_RIE	BIT(1)	/* RIN update interrupt enable */
#define MBOX_FLAGS_INT_UAE	BIT(8)	/* Urgent ACK interrupt enable */
#define MBOX_ALL_INTRS		(MBOX_FLAGS_INT_COE | \
				 MBOX_FLAGS_INT_RIE | \
				 MBOX_FLAGS_INT_UAE)

/* Status */
#define MBOX_STATUS_UA_MSK	BIT(8)

#define MBOX_CMD_HEADER(client, id, len, cmd)		   \
	(((cmd) << MBOX_HDR_CMD_LSB) & MBOX_HDR_CMD_MSK) | \
	(((len) << MBOX_HDR_LEN_LSB) & MBOX_HDR_LEN_MSK) | \
	(((id) << MBOX_HDR_ID_LSB) & MBOX_HDR_ID_MSK) 	 | \
	(((client) << MBOX_HDR_CLIENT_LSB) & MBOX_HDR_CLIENT_MSK)

#define MBOX_RESP_ERR_GET(resp)				\
	(((resp) & MBOX_HDR_CMD_MSK) >> MBOX_HDR_CMD_LSB)
#define MBOX_RESP_LEN_GET(resp)			\
	(((resp) & MBOX_HDR_LEN_MSK) >> MBOX_HDR_LEN_LSB)
#define MBOX_RESP_ID_GET(resp)				\
	(((resp) & MBOX_HDR_ID_MSK) >> MBOX_HDR_ID_LSB)
#define MBOX_RESP_CLIENT_GET(resp)			\
	(((resp) & MBOX_HDR_CLIENT_MSK) >> MBOX_HDR_CLIENT_LSB)

/* Response error list */
typedef enum
{
	/* CMD completed succesfully, but check resp ARGS for any errors */
	MBOX_RESP_STATOK = 0,
	/* CMD is incorrectly formatted in some way */
	MBOX_RESP_INVALID_COMMAND = 1,
	/* BootROM Command code not undesrtood */
	MBOX_RESP_UNKNOWN_BR = 2,
	/* CMD code not recognized by firmware */
	MBOX_RESP_UNKNOWN = 3,
	/* Indicates that the device is not configured */
	MBOX_RESP_NOT_CONFIGURED = 256,
	/* Indicates that the device is busy */
	MBOX_RESP_DEVICE_BUSY = 0x1FF,
	/* Indicates that there is no valid response available */
	MBOX_RESP_NO_VALID_RESP_AVAILABLE = 0x2FF,
	/* General Error */
	MBOX_RESP_ERROR = 0x3FF,
} ALT_SDM_MBOX_RESP_CODE;

/* Mailbox command list */
#define MBOX_RESTART		2
#define MBOX_CONFIG_STATUS	4
#define MBOX_RECONFIG_MSEL	7
#define MBOX_RECONFIG_STATUS	9
#define MBOX_QSPI_OPEN		50
#define MBOX_QSPI_CLOSE		51
#define MBOX_QSPI_DIRECT	59
#define MBOX_REBOOT_HPS		71

struct socfpga_mailbox {
	u32 cin;		/* command valid offset */
	u32 rout;		/* response output offset */
	u32 urg;		/* urgent command */
	u32 flags;		/* interrupt enables */
	u32 pad_0x10_0x1f[4];	/* 0x10 - 0x1F reserved */
	u32 cout;		/* command free offset */
	u32 rin;		/* respond valid offset */
	u32 pad_0x28;		/* 0x28 reserved */
	u32 status;		/* mailbox status */
	u32 pad_0x30_0x3f[4];	/* 0x30 - 0x3F reserved */
	u32 cmd_buf[MBOX_CMD_BUFFER_SIZE];	/* 0x40 - 0xBC circular command buffer to SDM */
	u32 resp_buf[MBOX_RESP_BUFFER_SIZE];	/* 0xC0 - 0xFF circular response buffer */
};

/* Use define other than put into struct socfpga_mailbox to save spaces */
#define MBOX_DOORBELL_TO_SDM_REG	(SOCFPGA_MAILBOX_ADDRESS + 0x400)
#define MBOX_DOORBELL_FROM_SDM_REG	(SOCFPGA_MAILBOX_ADDRESS + 0x480)

int mbox_send_cmd(u8 id, u32 cmd, u32 len, u32 *arg, u8 urgent,
			u32 *resp_buf_len, u32 *resp_buf);
int mbox_init(void);

#ifdef CONFIG_CADENCE_QSPI
int mbox_qspi_close(void);
int mbox_qspi_open(void);
#endif

int mbox_reset_cold(void);

#endif /* _MAILBOX_S10_H_ */
