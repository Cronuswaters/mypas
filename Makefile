#!/usr/make -f
# Author: Eraldo P Marinho
# First version: Wed Oct 10 14:47:20 -03 2018
# Makefile for main directory of the MyPas project
#

projectname=mypas
projectdir=$(PWD)/
codesdir=$(projectdir)/src/
includedir=$(projectdir)/include/

default:
	(cd $(codesdir); make )

clean:
	(cd $(codesdir); make $@ )

mostlyclean:
	(cd $(codesdir); make $@ )

