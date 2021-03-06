#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran

# Macros
PLATFORM=GNU-Linux-x86

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Release/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../CVCMIServer.o \
	${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../../hch/CLodHandler.o \
	${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../CGameHandler.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/${PLATFORM}/vcmi_server

dist/Release/${PLATFORM}/vcmi_server: ${OBJECTFILES}
	${MKDIR} -p dist/Release/${PLATFORM}
	${LINK.cc} -o dist/Release/${PLATFORM}/vcmi_server ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../CVCMIServer.o: ../CVCMIServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/..
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../CVCMIServer.o ../CVCMIServer.cpp

${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../../hch/CLodHandler.o: ../../hch/CLodHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../../hch
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../../hch/CLodHandler.o ../../hch/CLodHandler.cpp

${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../CGameHandler.o: ../CGameHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/..
	$(COMPILE.cc) -O2 -o ${OBJECTDIR}/_ext/home/t0/vcmi/trunk/server/vcmi_server/../CGameHandler.o ../CGameHandler.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/${PLATFORM}/vcmi_server

# Subprojects
.clean-subprojects:
