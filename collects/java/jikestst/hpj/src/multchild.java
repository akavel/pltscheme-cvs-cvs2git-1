// This software is subject to the terms of the IBM Jikes Test Suite
// License Agreement available at the following URL:
// http://www.ibm.com/research/jikes.
// Copyright (C) 1996, 1999, International Business Machines Corporation
// and others.  All Rights Reserved.
// You must accept the terms of that agreement to use this software.

class multchild extends multparent {
  int valuechild;
  int valueparent;
  int getchild() {
    return valuechild;
  }

  int getvgrand() {
    return this.getgrand();
  }
}

