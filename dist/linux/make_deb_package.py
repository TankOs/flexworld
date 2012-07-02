#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os
import subprocess
import re
import shutil
from os import path

TARGET_PATH_PREFIX = "usr"
SCRIPT_PATH = path.abspath( path.dirname( __file__ ) )

# Read commandline arguments.
if len( sys.argv ) < 2:
	print u"Usage: make_deb_package.py CMAKE_CACHE_PATH [DISTRIBUTION]"
	sys.exit( 1 )

cmake_cache_path = sys.argv[1]

distro = ""
if len( sys.argv ) > 2:
	distro = sys.argv[2]

# Check CMake cache path.
if not path.exists( cmake_cache_path ):
	print u"Path does not exist: {0}".format( cmake_cache_path )
	sys.exit( 1 )

cmake_cache_path = path.abspath( cmake_cache_path )

# Collect system information.

# Architecture.
result = subprocess.check_output( ["arch"] ).strip()
arch = None

if "x86_64" in result:
	arch = "amd64"
elif "386" in result:
	arch = "i386"
else:
	print u"Unrecognized architecture: {0}".forrmat( result )
	sys.exit( 1 )

# Distribution.
if not distro:
	pv_file = open( "/proc/version", "r" )
	buf = pv_file.read()
	pv_file.close()

	if "Debian" in buf:
		distro = "debian"
	else:
		print u"Unrecognized distribution: {0}".format( buf )
		sys.exit( 1 )

# control file path.
control_file_path = path.join( SCRIPT_PATH, u"{0}.control".format( distro ) )

if not path.exists( control_file_path ):
	print u"control file missing: {0}".format( control_file_path )
	sys.exit( 1 )

# Read CMake cache file.
cache_file_path = path.join( cmake_cache_path, "CMakeCache.txt" )
if not path.exists( cache_file_path ):
	print u"CMake cache not found: {0}".format( cache_file_path )
	sys.exit( 1 )

f = open( cache_file_path, "r" )
cache_buffer = f.read()
f.close()

# Extract FlexWorld properties from cache.
PROPERTY_KEYS = ("FW_VERSION_MAJOR", "FW_VERSION_MINOR", "FW_VERSION_REVISION", "FW_VERSION_SUFFIX", "CMAKE_INSTALL_PREFIX")
property_values = {}

for key in PROPERTY_KEYS:
	match = re.search( r"^{0}:\w+=(.*)$".format( key ), cache_buffer, re.MULTILINE )

	if match is None:
		print u"Property {0} not found in cache.".format( key )
		sys.exit( 1 )
	
	property_values[key] = match.group( 1 )

# Version string.
fw_version = u"{0}.{1}.{2}".format(
	property_values["FW_VERSION_MAJOR"],
	property_values["FW_VERSION_MINOR"],
	property_values["FW_VERSION_REVISION"],
)

if len( property_values["FW_VERSION_SUFFIX"] ) > 0:
	fw_version += u"-{0}".format( property_values["FW_VERSION_SUFFIX"] )

# Package name.
package_name = u"flexworld_{0}_{1}".format( fw_version, arch )

# Create tmp directory (remove if already exists).
tmp_dir_path = path.join( SCRIPT_PATH, package_name )

if path.exists( tmp_dir_path ):
	shutil.rmtree( tmp_dir_path )

os.mkdir( tmp_dir_path )

# Copy files from CMake install manifest. Also count total filesize in bytes.
manifest_file_path = path.join( cmake_cache_path, "install_manifest.txt" )
total_filesize = 0

if not path.exists( manifest_file_path ):
	print u"CMake install manifest not found: {0}".format( manifest_file_path )
	sys.exit( 1 )

f = open( manifest_file_path, "r" )

for line in f:
	line = line.strip()
	source_path = path.dirname( line )

	# Build destination path.
	destination_path = path.dirname(
		path.join(
			tmp_dir_path,
			TARGET_PATH_PREFIX,
			line[len( property_values["CMAKE_INSTALL_PREFIX"] ) + (0 if property_values["CMAKE_INSTALL_PREFIX"][-1] == "/" else 1):],
		)
	)

	# Make directories.
	try:
		os.makedirs( destination_path )
	except:
		pass

	# Copy file, ignore paths and symlinks.
	if not path.isdir( line ) and not path.islink( line ):
		shutil.copy(
			line,
			path.join( destination_path, path.basename( line ) ),
		)

		total_filesize += path.getsize( line )

f.close()

# Create Debian structure.
debian_path = path.join( tmp_dir_path, "DEBIAN" )
os.mkdir( debian_path )

# Copy changelog file.
shutil.copy( path.join( SCRIPT_PATH, "changelog" ), debian_path )

# Copy menu file.
menu_path = path.join( tmp_dir_path, "usr", "share", "applications" )

os.makedirs( menu_path )
shutil.copy( path.join( SCRIPT_PATH, "flexworld.desktop" ), path.join( menu_path, "flexworld.desktop" ) )

# Open control file and replace identifiers.
f = open( control_file_path, "r" )
control_buffer = f.read()
f.close()

control_buffer = control_buffer.replace( "@VERSION@", fw_version )
control_buffer = control_buffer.replace( "@INSTALLED_SIZE@", str( total_filesize / 1024 ) )

f = open( path.join( debian_path, "control" ), "w" )
f.write( control_buffer )
f.close()

# Build package.
subprocess.call( ["dpkg", "-b", tmp_dir_path] )
