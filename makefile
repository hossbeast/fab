# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2021 Todd Freed <todd.freed@gmail.com>. All rights reserved.

all:

include config.mak
include makefile.common

all:
	$(MAKE) -C libxlinux
	$(MAKE) -C libcommon
	$(MAKE) -C libnarrator
	$(MAKE) -C libdescriptor
	$(MAKE) -C libvalyria
	$(MAKE) -C libyyutil
	$(MAKE) -C liblorien
	$(MAKE) -C libbacon
	$(MAKE) -C libmoria
	$(MAKE) -C bacon
	$(MAKE) -C libbam
	$(MAKE) -C bam
	$(MAKE) -C bamd

install:
	install -m755 -d "${INSTALLDIR}/${BINDIR}"
	install -m755 -t "${INSTALLDIR}/${BINDIR}" "${SRC_PATH}/bam/bam"
	install -m755 -t "${INSTALLDIR}/${BINDIR}" "${SRC_PATH}/bamd/bamd"
	install -m755 -t "${INSTALLDIR}/${BINDIR}" "${SRC_PATH}/bacon/bacon"
	install -m755 -d "${INSTALLDIR}/${SHAREDIR}/bam/doc"
	install -m644 -t "${INSTALLDIR}/${SHAREDIR}/bam/doc" "${SRC_PATH}/README.md"
	install -m644 -t "${INSTALLDIR}/${SHAREDIR}/bam/doc" "${SRC_PATH}/CHANGELOG"
	install -m755 -d "${INSTALLDIR}/${SHAREDIR}/bam/config"
	install -m755 -t "${INSTALLDIR}/${SHAREDIR}/bam/config" "${SRC_PATH}/etc/config.bam"
	install -m755 -t "${INSTALLDIR}/${SHAREDIR}/bam/config" "${SRC_PATH}/etc/bam/client_config"
	install -m755 -d "${INSTALLDIR}/${SHAREDIR}/licenses/bam"
	install -m644 -t "${INSTALLDIR}/${SHAREDIR}/licenses/bam" "${SRC_PATH}/LICENSE"
	install -m755 -d "${INSTALLDIR}/${SHAREDIR}/bam/modules"
	cp -dR "${SRC_PATH}/builtin-modules" "${INSTALLDIR}/${SHAREDIR}/bam/modules"
	cp -dR "${SRC_PATH}/builtin-plugins" "${INSTALLDIR}/${SHAREDIR}/bam/plugins"
