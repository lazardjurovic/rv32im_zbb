   import uvm_pkg::*;      // import the UVM library   
 `include "uvm_macros.svh" // Include the UVM macros

class cpu_config extends uvm_object;

   uvm_active_passive_enum is_active = UVM_ACTIVE;
   
   `uvm_object_utils_begin (cpu_config)
      `uvm_field_enum(uvm_active_passive_enum, is_active, UVM_DEFAULT)
   `uvm_object_utils_end

   function new(string name = "cpu_config");
      super.new(name);
   endfunction

endclass : cpu_config