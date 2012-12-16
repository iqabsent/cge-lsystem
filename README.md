# MScCGE-gp-2011 #
==============

Title: Willem L-System

Draws lines based on text in config files...

By: Willem B. van der Merwe
33245888

!!Requires DirectX 9!!

# Config files: #
Config files are plain-text files and are located in /cfgs.
The app is currently set up to read up to 8 config files during run-time using the number keys 1-8;
They are read line by line and interpreted as follows:
If a line contains ->, it's a production rule. It uses the first character, and the text after -> as a string.
If a line starts with n, this is used as the initial int value representing the generation to render.
If a line starts with s, this is used as the initial float scale to render at.
If a line starts with a, this is used as the initial float angle to branch at.
If none of the above conditions are met, the line is used as axiom.

# Code layout: #

## rtvs.h ##
Header file containing code supplied as RTVS, along with lots of modifications by myself to make it useful.

## main.cpp ##
Main Windows application file