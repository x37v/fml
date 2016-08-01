#include "defines.h"

float note_to_freq[128] = {
  8.175798915643707,//0
  8.661957218027252,//1
  9.177023997418988,//2
  9.722718241315029,//3
  10.300861153527183,//4
  10.913382232281373,//5
  11.562325709738575,//6
  12.249857374429663,//7
  12.978271799373287,//8
  13.75,//9
  14.567617547440307,//10
  15.433853164253883,//11
  16.351597831287414,//12
  17.323914436054505,//13
  18.354047994837977,//14
  19.445436482630058,//15
  20.601722307054366,//16
  21.826764464562746,//17
  23.12465141947715,//18
  24.499714748859326,//19
  25.956543598746574,//20
  27.5,//21
  29.13523509488062,//22
  30.86770632850775,//23
  32.70319566257483,//24
  34.64782887210901,//25
  36.70809598967594,//26
  38.890872965260115,//27
  41.20344461410875,//28
  43.653528929125486,//29
  46.2493028389543,//30
  48.999429497718666,//31
  51.91308719749314,//32
  55.0,//33
  58.27047018976124,//34
  61.7354126570155,//35
  65.40639132514966,//36
  69.29565774421802,//37
  73.41619197935188,//38
  77.78174593052023,//39
  82.4068892282175,//40
  87.30705785825097,//41
  92.4986056779086,//42
  97.99885899543733,//43
  103.82617439498628,//44
  110.0,//45
  116.54094037952248,//46
  123.47082531403103,//47
  130.8127826502993,//48
  138.59131548843604,//49
  146.8323839587038,//50
  155.56349186104046,//51
  164.81377845643496,//52
  174.61411571650194,//53
  184.9972113558172,//54
  195.99771799087463,//55
  207.65234878997256,//56
  220.0,//57
  233.08188075904496,//58
  246.94165062806206,//59
  261.6255653005986,//60
  277.1826309768721,//61
  293.6647679174076,//62
  311.1269837220809,//63
  329.6275569128699,//64
  349.2282314330039,//65
  369.9944227116344,//66
  391.99543598174927,//67
  415.3046975799451,//68
  440.0,//69
  466.1637615180899,//70
  493.8833012561241,//71
  523.2511306011972,//72
  554.3652619537442,//73
  587.3295358348151,//74
  622.2539674441618,//75
  659.2551138257398,//76
  698.4564628660078,//77
  739.9888454232688,//78
  783.9908719634985,//79
  830.6093951598903,//80
  880.0,//81
  932.3275230361799,//82
  987.7666025122483,//83
  1046.5022612023945,//84
  1108.7305239074883,//85
  1174.6590716696303,//86
  1244.5079348883237,//87
  1318.5102276514797,//88
  1396.9129257320155,//89
  1479.9776908465376,//90
  1567.981743926997,//91
  1661.2187903197805,//92
  1760.0,//93
  1864.6550460723597,//94
  1975.533205024496,//95
  2093.004522404789,//96
  2217.4610478149766,//97
  2349.31814333926,//98
  2489.0158697766474,//99
  2637.02045530296,//100
  2793.825851464031,//101
  2959.955381693075,//102
  3135.9634878539946,//103
  3322.437580639561,//104
  3520.0,//105
  3729.3100921447194,//106
  3951.066410048992,//107
  4186.009044809578,//108
  4434.922095629953,//109
  4698.63628667852,//110
  4978.031739553295,//111
  5274.04091060592,//112
  5587.651702928062,//113
  5919.91076338615,//114
  6271.926975707989,//115
  6644.875161279122,//116
  7040.0,//117
  7458.620184289437,//118
  7902.132820097988,//119
  8372.018089619156,//120
  8869.844191259906,//121
  9397.272573357044,//122
  9956.06347910659,//123
  10548.081821211836,//124
  11175.303405856126,//125
  11839.8215267723,//126
  12543.853951415975//127
};

namespace fm {
  float midi_note_to_freq(float midi_note) {
    int index = midi_note;
    if (index < 0)
      return note_to_freq[0];
    else if (index > 127)
      return note_to_freq[127];
    return note_to_freq[index];
  }
}

