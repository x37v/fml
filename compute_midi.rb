#!/usr/bin/env ruby

def freq(m)
  27.5 * 2 ** ((m - 21.0) / 12.0)
end

v = (-127..127).collect { |i| [i, freq(i)] }


puts '#include <arm_math.h>'
puts "namespace {"
puts "  float32_t note_to_freq[#{v.length()}] = {"
puts "    " + v.collect { |i,f| f.to_s }.join(",\n    ")
puts "  };"
puts "  arm_linear_interp_instance_f32 interp_instance = {#{v.length()}, #{v[0][0]}, 1, note_to_freq};"
puts "}"


puts "\nnamespace fm {"
puts "  float midi_note_to_freq(float midi_note) { return arm_linear_interp_f32(&interp_instance, midi_note); }"
puts "}"
