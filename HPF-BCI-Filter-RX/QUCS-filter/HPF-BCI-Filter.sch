<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-300,1076,1020,1.61051,0,333>
  <Grid=10,10,1>
  <DataSet=HPF-BCI-Filter.dat>
  <DataDisplay=HPF-BCI-Filter.dpl>
  <OpenDisplay=1>
  <Script=HPF-BCI-Filter.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Pac P1 1 290 180 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 290 210 0 0 0 0>
  <GND * 1 540 210 0 0 0 0>
  <GND * 1 680 210 0 0 0 0>
  <GND * 1 820 210 0 0 0 0>
  <Pac P2 1 930 180 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 930 210 0 0 0 0>
  <.SP SP1 1 300 280 0 67 0 0 "log" 1 "300kHz" 1 "30MHz" 1 "201" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Eqn Eqn1 1 520 290 -28 15 0 0 "dBS21=dB(S[2,1])" 1 "dBS11=dB(S[1,1])" 1 "yes" 0>
  <C C1 1 470 100 -27 10 0 0 "2.2nF" 1 "" 0 "neutral" 0>
  <C C2 1 610 100 -27 10 0 0 "820" 1 "" 0 "neutral" 0>
  <C C3 1 750 100 -27 10 0 0 "820" 1 "" 0 "neutral" 0>
  <C C4 1 870 100 -27 10 0 0 "2.2nF" 1 "" 0 "neutral" 0>
  <L L1 1 540 180 17 -26 0 1 "2.2uH" 1 "" 0>
  <L L2 1 680 180 17 -26 0 1 "2.2uH" 1 "" 0>
  <L L3 1 820 180 17 -26 0 1 "2.2uH" 1 "" 0>
</Components>
<Wires>
  <290 100 290 150 "" 0 0 0 "">
  <290 100 440 100 "" 0 0 0 "">
  <540 100 540 150 "" 0 0 0 "">
  <680 100 680 150 "" 0 0 0 "">
  <820 100 820 150 "" 0 0 0 "">
  <500 100 540 100 "" 0 0 0 "">
  <540 100 580 100 "" 0 0 0 "">
  <640 100 680 100 "" 0 0 0 "">
  <680 100 720 100 "" 0 0 0 "">
  <780 100 820 100 "" 0 0 0 "">
  <930 100 930 150 "" 0 0 0 "">
  <820 100 840 100 "" 0 0 0 "">
  <900 100 930 100 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 630 280 12 #000000 0 "Chebyshev high-pass filter \n 3MHz cutoff, pi-type, \n impedance matching 50 Ohm">
</Paintings>
