namespace fm {
  //smooths current -> target via a linear ramp where the ramp is defined
  //by a ramp from 0..1 with increments of increment.. so if you want to go
  //from 0..1 in 10ms at a sample rate of 44100 [calling lin_smooth at that rate]
  //increment should by 1.0 / (44100.0 * 0.010);
  float lin_smooth(float target, float current, float increment);
};
