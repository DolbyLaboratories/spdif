SPDIF v1.0
==========

Introduction
------------

The purpose of this command line tool is to support the conversion of an elementary stream file into a format that can be placed directly onto an IEC 60958 (S/PDIF) and HDMI transport.

During the development of the CD player in the eighties it was identified that a consumer transport was required to carry digital audio between a compact disc player and a receiver. It was based on the AES3 professional digital audio standard. The full history can be found here: https://en.wikipedia.org/wiki/S/PDIF.
With the advent of the DVD and the standardization of compressed surround sound, the need emerged for a way to carry the new audio format over the existing consumer digital transport. The solution, standardized in IEC 61937, is analogous to SMPTE 337 on the professional. However, it is worth noting that there these two formats are not always compatible, for example, in the transport of Dolby Digital Plus.
The tool provided here only supports IEC 60958 and IEC 61937 and does not support the professional use cases. 

Building
--------

Two build options are provided. A GNU Makefile is provided for use on Unix systems. A Visual Studio 2003 project is provided for Windows systems which can be imported into newer versions of Visual Studio. Both the Makefile and the MSVS project file can be found in this directory.

Testing
-------

A test suite is provided to support verification of changes to the source code. This test suite uses most of the provided options to exercise the most important functionality of the tool. The test suite can be executed by running either test_spdif.sh or test_spdif.bat from the test sub-directory. The shell script ’.sh’ file is intended for UNIX system while the ‘.bat’ file is intended for Windows systems and was tested on Windows 7.
The first and only argument specifies the location of the executable under test. By default the Release directory in the root directory is used which is shared by the build system.
In cases where the reference files are found to be missing then the script attempts to recreate these using the original source files and a reference executable. 

Execution
---------
The command line options for the tool can be discovered by executing it at the command line using the '-help' option. When deformatting AC-3 and E-AC-3 files, the elementary streams produced are in 'big endian' format. However, when formatting, the tool can ingest either big or little endian format files.

Dolby Digital is a registered trademark of Dolby Licensing Corporation
SMPTE is a registered trademark of the Society of Motion Picture and Television Engineers
IEC is this context refers to the International Electrotechnical Commission