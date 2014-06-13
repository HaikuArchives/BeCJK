// BeSavager - Savager Workroom Development Toolkit for BeOS
// (C) Copyright by Anthony Lee <anthonylee@eLong.com> 2002
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
//
//
// Custom Cursor

static const char s_cursor_move_data[] = {
16,             /* 16x16 size */
1,              /* 1 bit */
2,3,            /* spot point is (2,3) */

/* such as 00000001 = 1 */
/*         00000011 = 3 */

/* cursor data */
112,0,72,0,72,0,39,-64,36,-72,18,84,16,2,120,66,
-104,-30,-127,82,99,-6,25,82,4,-32,2,64,1,0,0,0,

/* cursor mask */
112,0,120,0,120,0,63,-64,63,-8,31,-4,31,-2,127,-2,
-1,-2,-1,-2,127,-2,31,-2,7,-4,3,-8,1,-16,0,0};


const void *S_CURSOR_MOVE = s_cursor_move_data;

