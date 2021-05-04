# WER
WER and SER Calculation
Tutorial Video : https://www.youtube.com/watch?v=8rzxSgLGoYo
## Features
+ Support for wild string
+ Trim spaces everywhere
+ Each non alpha numeric as a word
+ Multiple words optional matching
## Build
+ aclocal
+ autoconf
+ automake --add-missing
+ ./configure
+ make
## Output
Ref: a [e c,b d] d 

Hyp: a e b d 

WER: 1 SER: 1

Ref: a [e c,] d 

Hyp: a e c d 

WER: 0 SER: 0

Ref: a [e c,b d] d 

Hyp: a e b d 

WER: 1 SER: 1

Ref: a [e c,] d 

Hyp: a d 

WER: 0 SER: 0

Ref: adv − ßnStör 

Hyp: adv − ßnStör 

WER: 0 SER: 0

Ref: 1 + 2 - 3 / 4 

Hyp: 1 + 2 - 3 / 4 

WER: 0 SER: 0

Ref: x [al,p,pqr,a] y 

Hyp: x pqr y 

WER: 0 SER: 0


FINAL WER = 0.0526316   FINAL SER = 0.2
