# BeCJK & History

![screenshot](screenshot.png)

BeCJK, is an input method for the legancy BeOS, first developed by
Anthony.Lee. This copy was obtained from Andrew Bachmann.

# Building

## Pre-requisite software

  - git, get current code of BeCJK
  - scons, tools for building BeCJK

## building from source

\~path/to/source\> scons -Q

# Install & Uninstall

## install from source

\~path/to/source\> scons -Q /boot/system/

## uninstall from source

\~path/to/source\> scons -c /boot/system/

# Components

The code of BeCJK is released as three parts, including:

  - besavager, known as Savager Workroom Development Toolkit for BeOS,
    as a shared library.
  - BeCJK, main code for BeCJK IME, which will be built as an Add-on for
    Haiku/BeOS.
  - dicts, several dictionaries for BeCJK, working as modules which
    could be loaded into BeCJK.

# CopyRights:

copyright: (C) 2001-2002 Anthony Lee \<<anthonylee@eLong.com>\>

  - BeSavager --- freeware under LGPL2
  - BeCJK-AddOn --- freeware under GPL2
  - BeCJK-Devel --- freeware under LGPL2
