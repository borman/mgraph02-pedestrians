TARGET = linear

TEMPLATE = lib
CONFIG += staticlib
QT = 

VPATH += liblinear-1.8 liblinear-1.8/blas
HEADERS = linear.h tron.h blas.h blasp.h
SOURCES = linear.cpp tron.cpp daxpy.c ddot.c dnrm2.c dscal.c

CONFIG += silent
