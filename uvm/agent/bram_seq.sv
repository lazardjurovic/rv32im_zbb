class bram_seq extends uvm_sequence#(bram_seq_item);

    `uvm_object_utils(bram_seq)
    `uvm_declare_p_sequencer(bram_sequencer)

    function new(string name = "bram_seq");
        super.new(name);
    endfunction

    virtual task pre_body();
    // ...
    endtask : pre_body

    // transaction generating logic in body
    virtual task body();

        bram_seq_item bram_it;
        // prvi korak kreiranje transakcije
        bram_it = bram_seq_item::type_id::create("bram_it");
        // drugi korak − start
        start_item(bram_it);
        // treci korak priprema
        // po potrebi moguce prosiriti sa npr. inline ogranicenjima
        //assert (bram_it.randomize());
        // cetvrti korak − nish
        finish_item(bram_it);

    // calls to uvm_do or uvm_do with macro
    // or start / nish item
    // ...
    endtask : body

    virtual task post_body();
    // ...
    endtask : post_body

endclass : bram_seq