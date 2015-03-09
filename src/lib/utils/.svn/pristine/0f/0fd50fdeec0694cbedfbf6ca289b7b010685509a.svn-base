/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __NOTES_H__
#define __NOTES_H__


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define NOTE_OCTAVE_MULTIPLER		(2)

// Octave 0 and 1 cannot be represented as ints.
// Tuning is based on A=440
typedef enum
{
   NOTE_C2        = (33  ),    // MIDI Note #24
   NOTE_C2_SHARP  = (35  ),    // MIDI Note #25
   NOTE_D2        = (37  ),    // MIDI Note #26
   NOTE_D2_SHARP  = (39  ),    // MIDI Note #27
   NOTE_E2        = (41  ),    // MIDI Note #28
   NOTE_F2        = (44  ),    // MIDI Note #29
   NOTE_F2_SHARP  = (46  ),    // MIDI Note #30
   NOTE_G2        = (49  ),    // MIDI Note #31
   NOTE_G2_SHARP  = (52  ),    // MIDI Note #32
   NOTE_A2        = (55  ),    // MIDI Note #33
   NOTE_A2_SHARP  = (58  ),    // MIDI Note #34
   NOTE_B2        = (62  ),    // MIDI Note #35
   NOTE_C3        = (65  ),    // MIDI Note #36
   NOTE_C3_SHARP  = (69  ),    // MIDI Note #37
   NOTE_D3        = (73  ),    // MIDI Note #38
   NOTE_D3_SHARP  = (78  ),    // MIDI Note #39
   NOTE_E3        = (82  ),    // MIDI Note #40
   NOTE_F3        = (87  ),    // MIDI Note #41
   NOTE_F3_SHARP  = (92  ),    // MIDI Note #42
   NOTE_G3        = (98  ),    // MIDI Note #43
   NOTE_G3_SHARP  = (104 ),    // MIDI Note #44
   NOTE_A3        = (110 ),    // MIDI Note #45
   NOTE_A3_SHARP  = (117 ),    // MIDI Note #46
   NOTE_B3        = (123 ),    // MIDI Note #47
   NOTE_C4        = (131 ),    // MIDI Note #48
   NOTE_C4_SHARP  = (139 ),    // MIDI Note #49
   NOTE_D4        = (147 ),    // MIDI Note #50
   NOTE_D4_SHARP  = (156 ),    // MIDI Note #51
   NOTE_E4        = (165 ),    // MIDI Note #52
   NOTE_F4        = (175 ),    // MIDI Note #53
   NOTE_F4_SHARP  = (185 ),    // MIDI Note #54
   NOTE_G4        = (196 ),    // MIDI Note #55
   NOTE_G4_SHARP  = (208 ),    // MIDI Note #56
   NOTE_A4        = (220 ),    // MIDI Note #57
   NOTE_A4_SHARP  = (233 ),    // MIDI Note #58
   NOTE_B4        = (245 ),    // MIDI Note #59
   NOTE_C5        = (262 ),    // MIDI Note #60
   NOTE_C5_SHARP  = (277 ),    // MIDI Note #61
   NOTE_D5        = (294 ),    // MIDI Note #62
   NOTE_D5_SHARP  = (311 ),    // MIDI Note #63
   NOTE_E5        = (330 ),    // MIDI Note #64
   NOTE_F5        = (349 ),    // MIDI Note #65
   NOTE_F5_SHARP  = (370 ),    // MIDI Note #66
   NOTE_G5        = (392 ),    // MIDI Note #67
   NOTE_G5_SHARP  = (415 ),    // MIDI Note #68
   NOTE_A5        = (440 ),    // MIDI Note #69
   NOTE_A5_SHARP  = (466 ),    // MIDI Note #70
   NOTE_B5        = (494 ),    // MIDI Note #71
   NOTE_C6        = (523 ),    // MIDI Note #72
   NOTE_C6_SHARP  = (554 ),    // MIDI Note #73
   NOTE_D6        = (587 ),    // MIDI Note #74
   NOTE_D6_SHARP  = (622 ),    // MIDI Note #75
   NOTE_E6        = (659 ),    // MIDI Note #76
   NOTE_F6        = (698 ),    // MIDI Note #77
   NOTE_F6_SHARP  = (740 ),    // MIDI Note #78
   NOTE_G6        = (784 ),    // MIDI Note #79
   NOTE_G6_SHARP  = (831 ),    // MIDI Note #80
   NOTE_A6        = (880 ),    // MIDI Note #81
   NOTE_A6_SHARP  = (932 ),    // MIDI Note #82
   NOTE_B6        = (988 ),    // MIDI Note #83
   NOTE_C7        = (1046),    // MIDI Note #84
   NOTE_C7_SHARP  = (1108),    // MIDI Note #85
   NOTE_D7        = (1174),    // MIDI Note #86
   NOTE_D7_SHARP  = (1244),    // MIDI Note #87
   NOTE_E7        = (1318),    // MIDI Note #88
   NOTE_F7        = (1396),    // MIDI Note #89
   NOTE_F7_SHARP  = (1480),    // MIDI Note #90
   NOTE_G7        = (1568),    // MIDI Note #91
   NOTE_G7_SHARP  = (1662),    // MIDI Note #92
   NOTE_A7        = (1760),    // MIDI Note #93
   NOTE_A7_SHARP  = (1864),    // MIDI Note #94
   NOTE_B7        = (1976),    // MIDI Note #95
   NOTE_C8        = (2093),    // MIDI Note #96
   NOTE_C8_SHARP  = (2217),    // MIDI Note #97
   NOTE_D8        = (2349),    // MIDI Note #98
   NOTE_D8_SHARP  = (2489),    // MIDI Note #99
   NOTE_E8        = (2637),    // MIDI Note #100
   NOTE_F8        = (2794),    // MIDI Note #101
   NOTE_F8_SHARP  = (2960),    // MIDI Note #102
   NOTE_G8        = (3136),    // MIDI Note #103
   NOTE_G8_SHARP  = (3322),    // MIDI Note #104
   NOTE_A8        = (3520),    // MIDI Note #105
   NOTE_A8_SHARP  = (3729),    // MIDI Note #106
   NOTE_B8        = (3951),    // MIDI Note #107
   NOTE_C9        = (4186),    // MIDI Note #108
   NOTE_C9_SHARP  = (4435),    // MIDI Note #109
   NOTE_D9        = (4699),    // MIDI Note #110
   NOTE_D9_SHARP  = (4978),    // MIDI Note #111
   NOTE_E9        = (5274),    // MIDI Note #112
   NOTE_F9        = (5588),    // MIDI Note #113
   NOTE_F9_SHARP  = (5920),    // MIDI Note #114
   NOTE_G9        = (6272),    // MIDI Note #115
   NOTE_G9_SHARP  = (6645),    // MIDI Note #116
   NOTE_A9        = (7040),    // MIDI Note #117
   NOTE_A9_SHARP  = (7459),    // MIDI Note #118
   NOTE_B9        = (7902),    // MIDI Note #119
   NOTE_C10       = (8372),    // MIDI Note #120
   NOTE_C10_SHARP = (8870),    // MIDI Note #121
   NOTE_D10       = (9397),    // MIDI Note #122
   NOTE_D10_SHARP = (9956),    // MIDI Note #123
   NOTE_E10       = (10548),   // MIDI Note #124
   NOTE_F10       = (11175),   // MIDI Note #125
   NOTE_F10_SHARP = (11840),   // MIDI Note #126
   NOTE_G10       = (12544),   // MIDI Note #127
}NOTE;


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/

#endif
