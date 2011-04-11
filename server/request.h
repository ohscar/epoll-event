#include <stdlib.h>
//==============================DFA states=================================//
static size_t _table[256]={
/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
        0,       0,       3,       0,       0,       3,       0,       0,
/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
        2,       2,       2,       2,       2,       2,       2,       2,
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
        2,       2,       0,       0,       0,       0,       0,       0,
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
        0,       0,       0,       0,       0,       1,       0,       1,
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
        0,       0,       0,       0,       1,       0,       0,       0,
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
        0,       0,       0,       0,       0,       0,       0,       0,
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
        0,       0,       0,       0,       0,       0,       0,       0,
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
        0,       0,       0,       0,       0,       0,       0,       0,
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
        0,       0,       0,       0,       0,       0,       0,       0,
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
        0,       0,       0,       0,       0,       0,       0,       0 
	};

struct request{
	char *data;
	char *method;
	char *body;
	size_t length;
	size_t pos;
};

struct request *request_new();
void parse_request(struct request *request);
void free_request(struct request *request);
