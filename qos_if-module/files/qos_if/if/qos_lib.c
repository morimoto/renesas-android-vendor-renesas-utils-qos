/*
 * Copyright (c) 2016 Renesas Electronics Corporation
 * Released under the MIT license
 * http://opensource.org/licenses/mit-license.php
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/ioctl.h>

#include "qos_api_local.h"

#define LOG_TAG "QOS-LIB"
#include <log/log.h>

/* #define QOS_DEBUG 1 */

#define QOS_INDEX (512)

struct qos_lib_handle {
	int fd;
};

int qos_lib_init(unsigned long *handle)
{
	struct qos_lib_handle *hdl;

	if (handle == NULL) {
		ALOGE("invalid parameter\n");
		return -1;
	}

	hdl = malloc(sizeof(*hdl));
	if (!hdl) {
		ALOGE("allcate memory\n");
		return -1;
	}

	hdl->fd = open("/dev/"QOS_DEVICE_NAME, O_RDONLY | O_NONBLOCK);
	if (hdl->fd == -1) {
		ALOGE("open %s\n", "/dev/"QOS_DEVICE_NAME);
		return -1;
	}

	*handle = (unsigned long)hdl;

	return 0;
}

int qos_lib_quit(unsigned long handle)
{
	struct qos_lib_handle *hdl = (struct qos_lib_handle *)handle;

	if (hdl) {
		close(hdl->fd);
		free(hdl);
	}

	return 0;
}

int qos_lib_setall(unsigned long handle,
		   unsigned char *fix_qos, unsigned char *be_qos)
{
	struct qos_lib_handle *hdl = (struct qos_lib_handle *)handle;
	int ret;
	struct qos_ioc_set_all_qos_param all_qos_param;

	if ((hdl == NULL) || (fix_qos == NULL) || (be_qos == NULL)) {
		ALOGE("%s: invalid parameter", __func__);
		return -1;
	}

	memset(&all_qos_param, 0x00, sizeof(all_qos_param));

	all_qos_param.fix_qos = fix_qos;
	all_qos_param.be_qos = be_qos;

	ret = ioctl(hdl->fd, QOS_IOCTL_SET_ALL_QOS, &all_qos_param);
	if (ret != 0)
		ALOGE("%s: ioctl(QOS_IOCTL_SET_ALL_QOS) failed.", __func__);
	else
		ALOGI("QoS : setall success!");

	return ret;
}

int qos_lib_setall_from_csv(unsigned long handle, char *read_file)
{
	struct qos_lib_handle *hdl = (struct qos_lib_handle *)handle;
	FILE *fp;
	unsigned char fix_qos[QOS_INDEX * 8] = {0};
	unsigned char be_qos[QOS_INDEX * 8] = {0};
	char buf[256] = {0};
	int master_id;
	unsigned char  ip_name[64], fix_address[64], be_address[64];
	unsigned long long fix_value, be_value;
	int fix_l, fix_band, be_l, be_band;
	int major, minor;
	int index = 0;

	if ((hdl == NULL) || (read_file == NULL)) {
		ALOGE("%s: invalid parameter", __func__);
		return -1;
	}

	fp = fopen(read_file, "r");
	if (fp == NULL) {
		ALOGE("%s: open %s failed", __func__, read_file);
		return -1;
	}

	if (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "ver %d.%d", &major, &minor) == 2) {
			ALOGI("QoS : CSV file ver %d.%d", major, minor);
		} else {
			rewind(fp);
		}
	}

	while (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "%d, %[^,], %[^,], %llx, %[^,], %llx, %d, %d, %d, %d",
			&master_id, ip_name, fix_address,
			&fix_value, be_address, &be_value,
			&fix_l, &fix_band, &be_l, &be_band) == 10) {
			;
		} else if (sscanf(buf, "%d, %[^,], %[^,], %llx, %[^,], %llx",
			&master_id, ip_name, fix_address,
			&fix_value, be_address, &be_value) == 6) {
#ifdef QOS_DEBUG
			fix_l = fix_band = be_l = be_band = 0;
#else
			;
#endif
		} else {
			ALOGW("%s: data-format is wrong!", __func__);
		}
#ifdef QOS_DEBUG
		ALOGD("index:%d", index);
		ALOGD(" FIX:0x%016llx BE:0x%016llx", fix_value, be_value);
		ALOGD(" FIX:%d %d BE:%d %d", fix_l, fix_band, be_l, be_band);
#endif
		memcpy(fix_qos + (8 * index), &fix_value,
			sizeof(unsigned long long));

		memcpy(be_qos + (8 * index), &be_value,
			sizeof(unsigned long long));

		index++;
		if (index >= QOS_INDEX)
			break;
	}

	fclose(fp);

	return qos_lib_setall(handle, fix_qos,  be_qos);
}

int qos_lib_switch(unsigned long handle)
{
	struct qos_lib_handle *hdl = (struct qos_lib_handle *)handle;
	int ret;

	if (hdl == NULL) {
		ALOGE("%s: invalid parameter", __func__);
		return -1;
	}

	ret = ioctl(hdl->fd, QOS_IOCTL_SWITCH_MEMBANK, NULL);
	if (ret != 0) {
		ALOGE("%s: ioctl(QOS_IOCTL_SWITCH_MEMBANK) failed.", __func__);
		return -1;
	}

	return 0;
}

