// Bank 0:
4080 0000-0FEF: phrases-> notes
  64 0FF0-102F: bookmarks
  96 1030-108F: empty
 512 1090-128F: grooves
1024 1290-168F: song-> chainno (listed PU1,PU2,WAV,NOI for $00 .. $ff in order)
 512 1690-188F: tables-> envelope
// 1344 1890-1DCF: instrument->speech->words ($20*42)
//  168 1DD0-1E77: instr->speech->wordnames
//    2 1E78-1E79: mem initialized flag (set to “rb” on init)
 320 1E7A-1FB9: instr->names
  70 1FBA-1FFF: empty

// Bank 1:
  32 2000-201F: empty 
  32 2020-203F: table allocation table (byte per table, 1 if alloced, 0 else)
  64 2040-207F: instr alloc table (byte per instrument, 1 if alloced, 0 else)
2048 2080-287F: chains-> phraseno
2048 2880-307F: chains-> transposes
// 1024 3080-347F: instr->param
 512 3480-367F: tables-> transpose
 512 3680-387F: tables-> fx
 512 3880-3A7F: tables-> fx val
 512 3A80-3C7F: tables-> fx 2
 512 3C80-3E7F: tables-> fx 2 val
//    2 3E80-3E81: mem initialized flag (set to “rb” on init)
  32 3E82-3EA1: phrase allocation table (bit per phrase, 1 if alloced, 0 else)
  16 3EA2-3EB1: chain allocation table (bit per chain, 1 if alloced, 0 else)
//  256 3EB2-3FB1: softsynth params (16 parameter blocks of 16 bytes each)
   1 3FB2-3FB2: clock, hours
   1 3FB3-3FB3: clock, minutes
   1 3FB4-3FB4: tempo
   1 3FB5-3FB5: tune setting
   1 3FB6-3FB6: total clock, days
   1 3FB7-3FB7: total clock, hours
   1 3FB8-3FB8: total clock, minutes
   1 3FB9-3FB9: total clock, checksum (days+hours+minutes)
   1 3FBA-3FBA: key delay
   1 3FBB-3FBB: key repeat
   1 3FBC-3FBC: font
   1 3FBD-3FBD: sync setting
   1 3FBE-3FBE: colorset
   1 3FBF-3FBF: empty
   1 3FC0-3FC0: clone (0=deep, 1=slim)
   1 3FC1-3FC1: file changed?
//    1 3FC2-3FC2: power save
   1 3FC3-3FC3: prelisten
//    2 3FC4-3FC5: wave synth overwrite locks
  58 3FC6-3FFF: empty

// Bank 2:
4080 4000-4FEF: phrases->fx
4080 4FF0-5FDF: phrases->fx val
  32 5FE0-5FFF: empty

// Bank 3:
// 4096 6000-6FFF: 256 wave frames (each frame has 32 4-bit samples)
4080 7000-7FEF: phrases->instr
//    2 7FF0-7FF1: mem initialized flag (set to “rb” on init)
  13 7FF2-7FFE: empty
   1 7FFF-7FFF: version byte