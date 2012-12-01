LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := VBoxGuestR3Lib
LOCAL_CFLAGS:=-O2 -g
LOCAL_CFLAGS+=-DVBOX_WITH_HGCM -DVBOX_WITH_GUEST_PROPS -DIN_RING3 -DRT_OS_LINUX -DIPRT_BLDCFG_SCM_REV=77245 -DIPRT_BLDCFG_VERSION_STRING="\"4.1.12\"" -DIPRT_BLDCFG_VERSION_MAJOR=4 -DIPRT_BLDCFG_VERSION_MINOR=0 -DIPRT_BLDCFG_VERSION_BUILD=12 -DIPRT_BLDCFG_TARGET="\"linux\"" -DIPRT_BLDCFG_TARGET_ARCH="\"x86\"" -DIPRT_BLDCFG_TYPE="\"release\""
LOCAL_C_INCLUDES += device/androVM/vbox86/virtualbox/include
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibAdditions.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibAutoLogon.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibBalloon.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibClipboard.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibCoreDump.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3Lib.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibCpuHotPlug.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibCredentials.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibDaemonize.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibEvent.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibGR.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibGuestCtrl.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibGuestProp.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibHostVersion.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibLog.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibMisc.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibModule.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibMouse.cpp
#LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibRuntimeXF86.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibSeamless.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibSharedFolders.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibStat.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibTime.cpp
LOCAL_SRC_FILES += VBoxGuestR3Lib/VBoxGuestR3LibVideo.cpp

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := RuntimeGuestR3
LOCAL_CFLAGS:=-O2 -g
LOCAL_CFLAGS+=-DVBOX_WITH_HGCM -DVBOX_WITH_GUEST_PROPS -DVBOX_WITH_SHARED_FOLDERS -DIN_RING3 -DRT_OS_LINUX -DIPRT_BLDCFG_SCM_REV=77245 -DIPRT_BLDCFG_VERSION_STRING="\"4.1.12\"" -DIPRT_BLDCFG_VERSION_MAJOR=4 -DIPRT_BLDCFG_VERSION_MINOR=0 -DIPRT_BLDCFG_VERSION_BUILD=12 -DIPRT_BLDCFG_TARGET="\"linux\"" -DIPRT_BLDCFG_TARGET_ARCH="\"x86\"" -DIPRT_BLDCFG_TYPE="\"release\""
LOCAL_C_INCLUDES += device/androVM/vbox86/virtualbox/include external/openssl/include external/zlib external/liblzf
LOCAL_SRC_FILES = \
	common/alloc/alloc.cpp \
	common/alloc/heapsimple.cpp \
	common/alloc/heapoffset.cpp \
	common/alloc/memcache.cpp \
	common/alloc/memtracker.cpp \
	common/checksum/adler32.cpp \
	common/checksum/crc32.cpp \
	common/checksum/crc64.cpp \
	common/checksum/md5.cpp \
	common/checksum/md5str.cpp \
	common/checksum/ipv4.cpp \
	common/checksum/ipv6.cpp \
	common/checksum/manifest.cpp \
	common/checksum/manifest2.cpp \
	common/checksum/manifest3.cpp \
	common/checksum/manifest-file.cpp \
	common/checksum/RTSha1Digest.cpp \
	common/checksum/sha1.cpp \
	common/checksum/sha1str.cpp \
	common/checksum/sha256.cpp \
	common/checksum/sha256str.cpp \
	common/checksum/sha512.cpp \
	common/checksum/sha512str.cpp \
	common/dbg/dbg.cpp \
	common/dbg/dbgas.cpp \
	common/dbg/dbgmod.cpp \
	common/dbg/dbgmodcontainer.cpp \
	common/dbg/dbgmodnm.cpp \
	common/dvm/dvm.cpp \
	common/dvm/dvmbsdlabel.cpp \
	common/dvm/dvmgpt.cpp \
	common/dvm/dvmmbr.cpp \
	common/err/errinfo.cpp \
	common/err/errmsg.cpp \
	common/err/RTErrConvertFromErrno.cpp \
	common/err/RTErrConvertToErrno.cpp \
	common/ldr/ldr.cpp \
	common/ldr/ldrELF.cpp \
	common/ldr/ldrEx.cpp \
	common/ldr/ldrFile.cpp \
	common/ldr/ldrNative.cpp \
	common/ldr/ldrPE.cpp \
	common/log/log.cpp \
	common/log/logellipsis.cpp \
	common/log/logrel.cpp \
	common/log/logrelellipsis.cpp \
	common/log/logcom.cpp \
	common/log/logformat.cpp \
	common/log/tracebuf.cpp \
	common/log/tracedefault.cpp \
	common/misc/RTAssertMsg1Weak.cpp \
	common/misc/RTAssertMsg2.cpp \
	common/misc/RTAssertMsg2Add.cpp \
	common/misc/RTAssertMsg2AddWeak.cpp \
	common/misc/RTAssertMsg2AddWeakV.cpp \
	common/misc/RTAssertMsg2Weak.cpp \
	common/misc/RTAssertMsg2WeakV.cpp \
	common/misc/RTFileOpenF.cpp \
	common/misc/RTFileOpenV.cpp \
	common/misc/RTMemWipeThoroughly.cpp \
	common/misc/assert.cpp \
	common/misc/buildconfig.cpp \
	common/misc/cidr.cpp \
	common/misc/getopt.cpp \
	common/misc/getoptargv.cpp \
	common/misc/handle.cpp \
	common/misc/handletable.cpp \
	common/misc/handletablectx.cpp \
	common/misc/handletablesimple.cpp \
	common/misc/lockvalidator.cpp \
	common/misc/message.cpp \
	common/misc/once.cpp \
	common/misc/req.cpp \
	common/misc/sanity-c.c \
	common/misc/sanity-cpp.cpp \
	common/misc/semspingpong.cpp \
	common/misc/sg.cpp \
	common/misc/circbuf.cpp \
	common/misc/thread.cpp \
	common/misc/term.cpp \
	common/path/rtPathRootSpecLen.cpp \
	common/path/rtPathVolumeSpecLen.cpp \
	common/path/RTPathAbsDup.cpp \
	common/path/RTPathAbsEx.cpp \
	common/path/RTPathAbsExDup.cpp \
	common/path/RTPathAppend.cpp \
	common/path/RTPathAppendEx.cpp \
	common/path/RTPathChangeToDosSlashes.cpp \
	common/path/RTPathChangeToUnixSlashes.cpp \
	common/path/RTPathCopyComponents.cpp \
	common/path/RTPathCountComponents.cpp \
	common/path/RTPathExt.cpp \
	common/path/RTPathFilename.cpp \
	common/path/RTPathHaveExt.cpp \
	common/path/RTPathHavePath.cpp \
	common/path/RTPathJoin.cpp \
	common/path/RTPathJoinA.cpp \
	common/path/RTPathJoinEx.cpp \
	common/path/RTPathParse.cpp \
	common/path/RTPathRealDup.cpp \
	common/path/RTPathStartsWithRoot.cpp \
	common/path/RTPathStripExt.cpp \
	common/path/RTPathStripFilename.cpp \
	common/path/RTPathStripTrailingSlash.cpp \
	common/path/RTPathTraverseList.cpp \
	common/path/comparepaths.cpp \
	common/rand/rand.cpp \
	common/rand/randadv.cpp \
	common/rand/randparkmiller.cpp \
	common/sort/RTSortIsSorted.cpp \
	common/sort/RTSortApvIsSorted.cpp \
	common/sort/shellsort.cpp \
	common/string/RTStrCat.cpp \
	common/string/RTStrCatEx.cpp \
	common/string/RTStrCatP.cpp \
	common/string/RTStrCatPEx.cpp \
	common/string/RTStrCmp.cpp \
	common/string/RTStrCopy.cpp \
	common/string/RTStrCopyEx.cpp \
	common/string/RTStrCopyP.cpp \
	common/string/RTStrCopyPEx.cpp \
	common/string/RTStrNCmp.cpp \
	common/string/RTStrNLen.cpp \
	common/string/RTStrNLenEx.cpp \
	common/string/RTStrPrintHexBytes.cpp \
	common/string/RTStrStr.cpp \
	common/string/base64.cpp \
	common/string/simplepattern.cpp \
	common/string/straprintf.cpp \
	common/string/strformat.cpp \
	common/string/strformatnum.cpp \
	common/string/strformatrt.cpp \
	common/string/strformattype.cpp \
	common/string/strhash1.cpp \
	common/string/stringalloc.cpp \
	common/string/strprintf.cpp \
	common/string/strspace.cpp \
	common/string/strstrip.cpp \
	common/string/strtonum.cpp \
	common/string/strversion.cpp \
	common/string/uni.cpp \
	common/string/unidata.cpp \
	common/string/utf-16.cpp \
	common/string/utf-8.cpp \
	common/string/utf-8-case.cpp \
	common/string/ministring.cpp \
	common/table/avlgcptr.cpp \
	common/table/avlhcphys.cpp \
	common/table/avlgcphys.cpp \
	common/table/avllu32.cpp \
	common/table/avlou32.cpp \
	common/table/avlogcphys.cpp \
	common/table/avlogcptr.cpp \
	common/table/avlohcphys.cpp \
	common/table/avloioport.cpp \
	common/table/avlpv.cpp \
	common/table/avlrgcptr.cpp \
	common/table/avlrogcphys.cpp \
	common/table/avlrogcptr.cpp \
	common/table/avlroioport.cpp \
	common/table/avlroogcptr.cpp \
	common/table/avlrpv.cpp \
	common/table/avlruintptr.cpp \
	common/table/avlrfoff.cpp \
	common/table/avlru64.cpp \
	common/table/avlu32.cpp \
	common/table/avluintptr.cpp \
	common/table/avlul.cpp \
	common/table/table.cpp \
	common/time/time.cpp \
	common/time/timeprog.cpp \
	common/time/timesup.cpp \
	common/vfs/vfsbase.cpp \
	common/vfs/vfschain.cpp \
	common/vfs/vfsiosmisc.cpp \
	common/vfs/vfsmemory.cpp \
	common/vfs/vfsmisc.cpp \
	common/vfs/vfsstdfile.cpp \
	common/zip/tar.cpp \
	common/zip/tarcmd.cpp \
	common/zip/tarvfs.cpp \
	common/zip/gzipvfs.cpp \
	common/zip/zip.cpp \
	generic/critsect-generic.cpp \
	generic/env-generic.cpp \
	generic/RTDirCreateTemp-generic.cpp \
	generic/RTDirCreateUniqueNumbered-generic.cpp \
	generic/RTEnvDupEx-generic.cpp \
	generic/RTFileCopy-generic.cpp \
	generic/RTFileQuerySize-generic.cpp \
	generic/RTFileReadAll-generic.cpp \
	generic/RTFileReadAllEx-generic.cpp \
	generic/RTFileReadAllByHandle-generic.cpp \
	generic/RTFileReadAllByHandleEx-generic.cpp \
	generic/RTFileReadAllFree-generic.cpp \
	generic/RTLogWriteStdErr-generic.cpp \
	generic/RTLogWriteStdOut-generic.cpp \
	generic/RTLogWriteUser-generic.cpp \
	generic/RTTimerLRCreate-generic.cpp \
	generic/mempool-generic.cpp \
	generic/semfastmutex-generic.cpp \
	generic/semxroads-generic.cpp \
	generic/spinlock-generic.cpp \
	generic/strcache-stubs-generic.cpp \
	generic/timerlr-generic.cpp \
	r3/alloc-ef.cpp \
	r3/alloc.cpp \
	r3/dir.cpp \
	r3/dir2.cpp \
	r3/fileio.cpp \
	r3/fs.cpp \
	r3/init.cpp \
	r3/isofs.cpp \
	r3/path.cpp \
	r3/process.cpp \
	r3/socket.cpp \
	r3/stream.cpp \
	r3/test.cpp \
	r3/testi.cpp \
	r3/tcp.cpp \
	r3/udp.cpp \
	r3/generic/semspinmutex-r3-generic.cpp \
	generic/RTDirQueryInfo-generic.cpp \
	generic/RTDirSetTimes-generic.cpp \
	generic/RTFileMove-generic.cpp \
	generic/RTLogWriteDebugger-generic.cpp \
	generic/RTProcDaemonize-generic.cpp \
 	generic/RTSemEventMultiWait-2-ex-generic.cpp \
 	generic/RTSemEventMultiWaitNoResume-2-ex-generic.cpp \
	generic/RTTimeLocalNow-generic.cpp \
	generic/RTTimerCreate-generic.cpp \
	generic/RTThreadSetAffinityToCpu-generic.cpp \
	generic/RTUuidCreate-generic.cpp \
	generic/mppresent-generic.cpp \
	generic/utf16locale-generic.cpp \
	generic/uuid-generic.cpp \
	r3/linux/RTThreadGetNativeState-linux.cpp \
	r3/linux/mp-linux.cpp \
	r3/linux/rtProcInitExePath-linux.cpp \
	r3/linux/sched-linux.cpp \
	r3/linux/sysfs.cpp \
	r3/linux/time-linux.cpp \
	r3/linux/thread-affinity-linux.cpp \
	r3/linux/RTProcIsRunningByName-linux.cpp \
	r3/linux/RTSystemQueryDmiString-linux.cpp \
	r3/posix/RTHandleGetStandard-posix.cpp \
	r3/posix/RTMemProtect-posix.cpp \
	r3/posix/RTPathUserHome-posix.cpp \
	r3/posix/RTSystemQueryOSInfo-posix.cpp \
        r3/posix/RTSystemQueryTotalRam-posix.cpp \
	r3/posix/RTTimeNow-posix.cpp \
	r3/posix/RTTimeSet-posix.cpp \
	r3/posix/rtmempage-exec-mmap-heap-posix.cpp \
	r3/posix/dir-posix.cpp \
	r3/posix/env-posix.cpp \
	r3/posix/errvars-posix.cpp \
	r3/posix/fileio-posix.cpp \
	r3/posix/filelock-posix.cpp \
	r3/posix/fs2-posix.cpp \
	r3/posix/fs3-posix.cpp \
	r3/posix/ldrNative-posix.cpp \
	r3/posix/path-posix.cpp \
	r3/posix/path2-posix.cpp \
	r3/posix/pathhost-posix.cpp \
	r3/posix/RTPathUserDocuments-posix.cpp \
	r3/posix/pipe-posix.cpp \
	r3/posix/poll-posix.cpp \
	r3/posix/process-posix.cpp \
	r3/posix/rand-posix.cpp \
	r3/posix/semrw-posix.cpp \
	r3/posix/symlink-posix.cpp \
	r3/posix/thread-posix.cpp \
	r3/posix/timelocal-posix.cpp \
	r3/posix/tls-posix.cpp \
	r3/posix/utf8-posix.cpp \
	r3/posix/thread2-posix.cpp \
# 	r3/linux/semevent-linux.cpp \
# 	r3/linux/semeventmulti-linux.cpp \
# 	r3/linux/semmutex-linux.cpp
#	r3/posix/RTFileQueryFsSizes-posix.cpp \
#	r3/posix/fileio2-posix.cpp \
#	r3/posix/fs-posix.cpp \
#	r3/posix/process-creation-posix.cpp \
#	r3/posix/timer-posix.cpp \

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES:= VBoxControl.cpp 

LOCAL_CFLAGS:=-O2 -g
LOCAL_CFLAGS+=-DVBOX_WITH_HGCM -DVBOX_WITH_GUEST_PROPS -DVBOX_WITH_SHARED_FOLDERS -DIN_RING3 -DRT_OS_LINUX -DIPRT_BLDCFG_SCM_REV=77245 -DIPRT_BLDCFG_VERSION_STRING="\"4.1.12\"" -DIPRT_BLDCFG_VERSION_MAJOR=4 -DIPRT_BLDCFG_VERSION_MINOR=0 -DIPRT_BLDCFG_VERSION_BUILD=12 -DIPRT_BLDCFG_TARGET="\"linux\"" -DIPRT_BLDCFG_TARGET_ARCH="\"x86\"" -DIPRT_BLDCFG_TYPE="\"release\""
LOCAL_C_INCLUDES += device/androVM/vbox86/virtualbox/include

LOCAL_STATIC_LIBRARIES := VBoxGuestR3Lib RuntimeGuestR3

LOCAL_MODULE:=VBoxControl
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
