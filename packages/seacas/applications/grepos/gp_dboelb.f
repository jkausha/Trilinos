C Copyright(C) 2011 National Technology & Engineering Solutions of
C Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
C NTESS, the U.S. Government retains certain rights in this software.
C 
C Redistribution and use in source and binary forms, with or without
C modification, are permitted provided that the following conditions are
C met:
C 
C * Redistributions of source code must retain the above copyright
C    notice, this list of conditions and the following disclaimer.
C           
C * Redistributions in binary form must reproduce the above
C   copyright notice, this list of conditions and the following
C   disclaimer in the documentation and/or other materials provided
C   with the distribution.
C                         
C * Neither the name of NTESS nor the names of its
C   contributors may be used to endorse or promote products derived
C   from this software without specific prior written permission.
C                                                 
C THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
C "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
C LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
C A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
C OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
C SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
C LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
C DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
C THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
C (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
C OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

C=======================================================================
      SUBROUTINE DBOELB (NDB, NELBS, NELBE,
     &   IDELB, NUMELB, NUMLNK, NUMATR, LINK, BLKTYP, ATRIB)
C=======================================================================

C   --*** DBOELB *** (EXOLIB) Write database element blocks
C   --
C   --DBOELB writes the element block information to the database.
C   --Some dynamic dimensioning is done.
C   --
C   --Parameters:
C   --   NDB - IN - the database file
C   --   NELBS, NELBE - IN - the number of first and last element blocks
C   --      to write
C   --   IDELB - IN - the element block IDs for each block
C   --   NUMELB - IN - the number of elements in each block
C   --   NUMLNK - IN - the number of nodes per element in each block
C   --   NUMATR - IN - the number of attributes in each block
C   --   LINK - IN - the connectivity for each block
C   --   ATRIB - IN - the attributes for each block
C   --
      include 'exodusII.inc'

      INTEGER NDB
      INTEGER IDELB(*)
      INTEGER NUMELB(*)
      INTEGER NUMLNK(*)
      INTEGER NUMATR(*)
      INTEGER LINK(*)
      REAL ATRIB(*)
      CHARACTER*(mxstln) BLKTYP(*)
      

      IELNK = 0
      IEATR = 0

      call expclb(ndb, idelb, blktyp, numelb, numlnk, numatr,
     *  .FALSE., ierr)
      IF (IERR .NE. 0) THEN
        CALL PRTERR ('FATAL', 'Error defining element blocks')
      END IF

      DO 100 NELB = NELBS, NELBE
         IELB  = NELB-NELBS+1
         ISLNK = IELNK + 1
         IELNK = IELNK + NUMLNK(IELB) * NUMELB(IELB)
         ISATR = IEATR + 1
         IEATR = IEATR + NUMATR(IELB) * NUMELB(IELB)

         if (numelb(ielb) .gt. 0 .and. numlnk(ielb) .gt. 0) then
           call expelc(ndb, idelb(ielb), link(islnk), ierr)
           IF (IERR .NE. 0) THEN
             CALL PRTERR ('FATAL',
     *         'Error writing element block connectivity')
           END IF
         end if

         if (numatr(ielb) .gt. 0) then
           call expeat(ndb, idelb(ielb), atrib(isatr), ierr)
           IF (IERR .NE. 0) THEN
             CALL PRTERR ('FATAL',
     *         'Error writing element block attributes')
           END IF
         end if
 100  CONTINUE

      RETURN
      END
