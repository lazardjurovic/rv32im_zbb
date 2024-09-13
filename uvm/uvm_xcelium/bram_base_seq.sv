/*
`ifndef BRAM_BASE_SEQ_SV
 `define BRAM_BASE_SEQ_SV

class bram_base_seq extends uvm_sequence#(bram_seq_item);

   `uvm_object_utils(bram_base_seq)
   `uvm_declare_p_sequencer(bram_sequencer)

   function new(string name = "bram_base_seq");
      super.new(name);
   endfunction

   // objections are raised in pre_body
   virtual task pre_body();
      uvm_phase phase = get_starting_phase();
      if (phase != null)
        phase.raise_objection(this, {"Running sequence '", get_full_name(), "'"});
   endtask : pre_body

   // objections are dropped in post_body
   virtual task post_body();
      uvm_phase phase = get_starting_phase();
      if (phase != null)
        phase.drop_objection(this, {"Completed sequence '", get_full_name(), "'"});
   endtask : post_body

virtual task pre_body();
   uvm_phase phase = uvm_phase::get_root_phase();
   if (phase != null)
      phase.raise_objection(this, {"Running sequence '", get_full_name(), "'"});
endtask : pre_body

virtual task post_body();
   uvm_phase phase = uvm_phase::get_root_phase();
   if (phase != null)
      phase.drop_objection(this, {"Completed sequence '", get_full_name(), "'"});
endtask : post_body


endclass : bram_base_seq

`endif
*/


`ifndef BRAM_BASE_SEQ_SV
 `define BRAM_BASE_SEQ_SV

class bram_base_seq extends uvm_sequence#(bram_seq_item);

   `uvm_object_utils(bram_base_seq)
   `uvm_declare_p_sequencer(bram_sequencer)

   function new(string name = "bram_base_seq");
      super.new(name);
   endfunction

   // The pre_body and post_body tasks are generally not used for objection management.
   // Instead, manage the sequence flow and item transactions here.

   virtual task body();
      // Add your sequence code here.
      // For example, you might start your sequence here and send items.
   endtask : body

endclass : bram_base_seq

`endif

