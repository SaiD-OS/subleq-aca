#include "arch.h"

#if defined(MONITOR) || defined(PROFILE)
//logging params
int addition = 0;
int subtraction = 0;
int shift = 0;
int logic = 0;
int multiplication = 0;
int slt_u = 0;
int jump = 0;
int load = 0;
int store = 0;
int branch = 0;
int subleqinstrcount = 0;

bool signalParser(uint8_t control, uint8_t mask) {
    return (control & mask) != 0;
}

void controlsignalsprint(uint8_t control, uint8_t printmask) {
    printf("Control Signals:\n");
    if(printmask & ZEROM)
        printf("zero signal: %s\t", signalParser(control, ZEROM) ? "true" : "false");
    if(printmask & MEMTOREGM)
        printf("memtoreg signal: %s\t", signalParser(control, MEMTOREGM) ? "true" : "false");
    if(printmask & ALUSRCM)
        printf("alusrc signal: %s\t", signalParser(control, ALUSRCM) ? "true" : "false");
    if(printmask & BRANCHM)
        printf("branch signal: %s\t", signalParser(control, BRANCHM) ? "true" : "false");
    if(printmask & MEMREADM)
        printf("memread signal: %s\t", signalParser(control, MEMREADM) ? "true" : "false");
    if(printmask & MEMWRITEM)
        printf("memwrite signal: %s\t", signalParser(control, MEMWRITEM) ? "true" : "false");
    if(printmask & REGWRITEM)
        printf("regwrite signal: %s\t", signalParser(control, REGWRITEM) ? "true" : "false");
    if(printmask & REGDESTM)
        printf("regdest signal: %s\t", signalParser(control, REGDESTM) ? "true" : "false");
    printf("\n");
}

void instructioninfo(uregint_t ir) {
    #ifdef PROFILE    
        uint8_t opcode = ir >> 26;        
        uint8_t rs = (ir >> 21) & RSM;        
        uint8_t rt = (ir >> 16) & RTM;
        uint8_t rd = (ir >> 11) & RDM;
        uint8_t funct = ir & FUNCTM;
        uregint_t target = ir & JUMPFM;
        immregint_t imm = ir & IMMEDM;
         //r type
        bool r_type = (opcode == 0x00) & (ir != NOOP);
        //j type
        bool j_cond = (opcode == 0x02);
        bool jal_cond = (opcode == 0x03);
        //b type    
        bool beq_cond = ((opcode == 0x04) & (rt != 0x0));
        bool bgez_cond = ((opcode == 0x01) & (rt == 0x1));
        bool bgezal_cond = ((opcode == 0x01) & (rt == 0x11));
        bool bgtz_cond = (opcode == 0x07);
        bool blez_cond = (opcode == 0x06);
        bool bltz_cond = ((opcode == 0x01) & (rt == 0x0));
        bool bltzal_cond = ((opcode == 0x01) & (rt == 0x10));
        bool bne_cond = (opcode == 0x05);   
        //i type
        bool addi_cond = (opcode == 0x08);
        bool addiu_cond = (opcode == 0x09);
        bool andi_cond = (opcode == 0x0C);
        bool lui_cond = (opcode == 0x0F);
        bool ori_cond = (opcode == 0x0D);
        bool slti_cond = (opcode == 0x0A);
        bool sltiu_cond = (opcode == 0x0B);
        bool xori_cond = (opcode == 0x0E);

        bool lb_cond = (opcode == 0x20);
        bool lh_cond = (opcode == 0x21);
        bool lw_cond = (opcode == 0x23);
        bool lbu_cond = (opcode == 0x24);
        bool lhu_cond = (opcode == 0x25);
        bool sb_cond = (opcode == 0x28);
        bool sh_cond = (opcode == 0x29);
        bool sw_cond = (opcode == 0x2B);

        bool mfco_cond = (opcode == 0x10) & (rs == 0x0);
        bool mtco_cond = (opcode == 0x10) & (rs == 0x4);

        bool jr_cond = (r_type & (funct == 0x08));
        bool jalr_cond = (r_type & (funct == 0x09));
        bool sys_cond = (r_type & (funct == 0x0C));
        bool break_cond = (r_type & (funct == 0x0D));
        
        bool add_cond = r_type &&  ((funct == 0x20) || (funct == 0x21));
        bool and_cond = r_type &&  ((funct == 0x24) || (funct == 0xC));
        bool nor_cond = r_type && (funct == 0x27);
        bool or_cond = r_type && ((funct == 0x25) || (funct == 0xD));
        bool slt_cond = r_type && ((funct == 0x2A) || (funct == 0x2B));
        bool sub_cond = r_type && ((funct == 0x22) || (funct == 0x23));
        bool xor_cond = r_type && ((funct == 0x26) || (funct == 0xE));
        bool sl_cond = r_type && ((funct == 0x0) || (funct == 0x4));
        bool sr_cond = r_type && ((funct == 0x3) || (funct == 0x7) || (funct == 0x2) || (funct == 0x6));
        bool div_cond = r_type && ((funct == 0x1A) || (funct == 0x1B));
        bool mult_cond = r_type && ((funct == 0x18) || (funct == 0x19));
        bool mfhi_cond = (r_type & (funct == 0x10));
        bool mflo_cond = (r_type & (funct == 0x12));

        #ifdef MONITOR
        printf("Instruction Info::\nopcode: %d, rs: %d, rt: %d, ", opcode, rs, rt);
        if(r_type) {
            printf("rd: %d, funct: %d\n", rd, funct);
        } else if(j_cond | jal_cond | jr_cond | jalr_cond) {
            printf("target: %d\n", target);
        } else {            
            printf("immediate value, :%d\n", imm);
        }
        #endif

        if (add_cond | addi_cond | addiu_cond) addition++;
        if (sub_cond) subtraction++;
        if (and_cond | andi_cond | or_cond | ori_cond | xor_cond | xori_cond | nor_cond) logic++;
        if (sl_cond | sr_cond) shift++;
        if (mult_cond | mfhi_cond | mflo_cond) multiplication++;
        if (slt_cond | slti_cond | sltiu_cond) slt_u++;
        if (jr_cond | j_cond | sys_cond | jal_cond | jalr_cond | break_cond) jump++;
        if (lb_cond | lh_cond | lw_cond | lbu_cond | lhu_cond) load++;
        if (sb_cond | sh_cond | sw_cond) store++;
        if (bltz_cond | bgez_cond | beq_cond | bne_cond | blez_cond | bgtz_cond) branch++;
    #endif
}
#endif


int stage = 0;

uregint_t pc, pcnext;

uregint_t get_value(uregint_t addr, bool memread) {
    uregint_t instr = 0;
    if(memread) {
        for(int i=0; i<INCVAL; i++){
            instr += ((memunit)MEM[addr + i]) << (REG_SIZE - (i+1)*8);
        }
    }
    return instr;
}

void store_value(uregint_t addr, regint_t val, bool memwrite) {
    if (memwrite) {
        for(int i=0; i<INCVAL; i++) {
            MEM[addr+i] = (val >> (REG_SIZE - (i+1)*8)) & 0x000000FF;
        }
	}
}

uregint_t incrementPC(uregint_t pc_var) {
    return pc_var + INCVAL;
}

regint_t addBranchAddr(regint_t pc_var, regint_t branchAddr) {
    return pc_var + branchAddr;
}

regint_t signExtn(immregint_t offset) {
    return (regint_t) offset;
}

void generateAluControl(uint8_t aluop, uint8_t funct, uint8_t *alucontrol) {
    bool aluopg2 = (aluop > 0x1);
    bool add_cond = (funct == 0x20) | (funct == 0x8) | (funct == 0x9) | (funct == 0x21);
    bool lui_cond = (funct == 0xF);
    bool nor_cond = (funct == 0x27);
    bool or_cond = (funct == 0x25) | (funct == 0xD);
    bool slt_cond = (funct == 0x2A) | (funct == 0xA) | (funct == 0xB) | (funct == 0x2B);
    bool sub_cond = (funct == 0x22) | (funct == 0x23);
    bool xor_cond = (funct == 0x26) | (funct == 0xE);
    bool sl_cond = (funct == 0x0) | (funct == 0x4);
    bool sr_cond = (funct == 0x3) | (funct == 0x7) | (funct == 0x2) | (funct == 0x6);
    bool div_cond = (funct == 0x1A) | (funct == 0x1B);
    bool mult_cond = (funct == 0x18) | (funct == 0x19);
    *alucontrol = 0x0;
    *alucontrol |= (aluopg2 & (or_cond | lui_cond | nor_cond | slt_cond | sr_cond | mult_cond)) ? 0x1 : 0x0;
    *alucontrol |= (aluopg2 & (add_cond | nor_cond | slt_cond | sub_cond | mult_cond | div_cond)) ? 0x2 : 0x0;
    *alucontrol |= (aluopg2 & (lui_cond | slt_cond | sub_cond | xor_cond)) ? 0x4 : 0x0;
    *alucontrol |= (aluopg2 & (sl_cond | sr_cond | div_cond | mult_cond)) ? 0x8 : 0x0;
    if(aluop == 0x0) 
        *alucontrol = 0x2;
    else if(aluop == 0x1)
        *alucontrol = 0x6;
}

regint_t alu(regint_t op1, regint_t op2, uint8_t alucontrol, uint16_t *control) {
    regint_t aluOut = 0;
    uregint_t u_op2 = (uregint_t) op2;
    regint_t and_out = op1 & (((*control & UNSIGNEDM) != 0) ? u_op2 : op2);
    regint_t or_out = op1 | (((*control & UNSIGNEDM) != 0) ? u_op2 : op2); 
    regint_t add_out = op1 + (((*control & UNSIGNEDM) != 0) ? u_op2 : op2);
    regint_t nor_out = ~or_out;
    regint_t xor_out = op1 ^ (((*control & UNSIGNEDM) != 0) ? u_op2 : op2);    
    regint_t lui_out = u_op2 << 16;
    regint_t sub_out = op1 - (((*control & UNSIGNEDM) != 0) ? u_op2 : op2);
    regint_t slt_out = (op1 < (((*control & UNSIGNEDM) != 0) ? u_op2 : op2)) ? 0x1 : 0x0;
    regint_t sl_out = u_op2 << op1;
    regint_t sr_out = u_op2 >> op1;
    regint_t divmultop2 = (((*control & UNSIGNEDM) != 0) ? u_op2 : op2);
    regint_t hidiv_out = op1 % ((divmultop2 == 0) ? 0x1: divmultop2);
    regint_t lodiv_out = op1 / ((divmultop2 == 0) ? 0x1: divmultop2);
    int64_t mult_out = op1 * divmultop2;
    regint_t himult_out = mult_out >> 32;
    regint_t lomult_out = mult_out & 0xFFFFFFFF;

    bool and_cond = (alucontrol == 0x0);
    bool or_cond = (alucontrol == 0x1);
    bool add_cond = (alucontrol == 0x2);
    bool nor_cond = (alucontrol == 0x3);
    bool xor_cond = (alucontrol == 0x4);
    bool lui_cond = (alucontrol == 0x5);
    bool sub_cond = (alucontrol == 0x6);
    bool slt_cond = (alucontrol == 0x7);
    bool sl_cond = (alucontrol == 0x8);
    bool sr_cond = (alucontrol == 0x9);
    
    aluOut = (and_cond ? and_out : 0x0) | 
                (or_cond ? or_out : 0x0) | 
                (add_cond ? add_out : 0x0) | 
                (nor_cond ? nor_out : 0x0) | 
                (xor_cond ? xor_out : 0x0) | 
                (lui_cond ? lui_out : 0x0) | 
                (sub_cond ? sub_out : 0x0) | 
                (slt_cond ? slt_out : 0x0) | 
                (sl_cond ? sl_out : 0x0) | 
                (sr_cond ? sr_out : 0x0);
    
    if(alucontrol == 0xA) {
        store_value(HI_LOC, hidiv_out, true);
        store_value(LO_LOC, lodiv_out, true);
    } else if(alucontrol == 0xB) {
        store_value(HI_LOC, himult_out, true);
        store_value(LO_LOC, lomult_out, true);
    }

    *control |= ((aluOut == 0) & (and_cond | or_cond | add_cond | nor_cond | xor_cond | lui_cond | sub_cond | slt_cond | sl_cond | sr_cond)) ? ZEROM : 0x0;

    return aluOut;    
}

regint_t bitreversalop(regint_t bitrop) {
    regint_t fin = 0;
    for(int i=0; i<32; i++) {
        fin <<= 1;
        fin |= (bitrop & 0x1);
        bitrop >>= 1;
    }
    return fin;
}

regint_t retmin(regint_t op1, regint_t op2) {
    if(op1 < op2)
        return op1;
    return op2;
}

regint_t retmax(regint_t op1, regint_t op2) {
    if(op1 > op2)
        return op1;
    return op2;
}

void generateControlSignals(uregint_t instr, uint16_t *control, uint8_t *aluop, uint8_t *branchflags, uregint_t *executesubleqstart) {
    uint8_t opCode = instr >> 26;
    uint8_t functvar = instr & FUNCTM;
    uint8_t rsvar = (instr >> 21) & RSM;
    uint8_t rtvar = (instr >> 16) & RTM;
    //r type
    bool r_type = (opCode == 0x00) & (instr != NOOP);
    //j type
    bool j_cond = (opCode == 0x02);
    bool jal_cond = (opCode == 0x03);
    //b type    
    bool beq_cond = ((opCode == 0x04) & (rtvar != 0x0));
    bool bgez_cond = ((opCode == 0x01) & (rtvar == 0x1));
    bool bgezal_cond = ((opCode == 0x01) & (rtvar == 0x11));
    bool bgtz_cond = (opCode == 0x07);
    bool blez_cond = (opCode == 0x06);
    bool bltz_cond = ((opCode == 0x01) & (rtvar == 0x0));
    bool bltzal_cond = ((opCode == 0x01) & (rtvar == 0x10));
    bool bne_cond = (opCode == 0x05);   
    //Set Branch
    *branchflags = beq_cond ? BEQM : 0x0 | bgez_cond ? BGEZM : 0x0 | bgezal_cond ? BGEZALM : 0x0 | bgtz_cond ? BGTZM : 0x0 | blez_cond ? BLEZM : 0x0 | bltz_cond ? BLTZM : 0x0 | bltzal_cond ? BLTZALM : 0x0 | bne_cond ? BNEM : 0x0; 
    //i type
    bool addi_cond = (opCode == 0x08);
    bool addiu_cond = (opCode == 0x09);
    bool andi_cond = (opCode == 0x0C);
    bool lui_cond = (opCode == 0x0F);
    bool ori_cond = (opCode == 0x0D);
    bool slti_cond = (opCode == 0x0A);
    bool sltiu_cond = (opCode == 0x0B);
    bool xori_cond = (opCode == 0x0E);

    bool lb_cond = (opCode == 0x20);
    bool lh_cond = (opCode == 0x21);
    bool lw_cond = (opCode == 0x23);
    bool lbu_cond = (opCode == 0x24);
    bool lhu_cond = (opCode == 0x25);
    bool sb_cond = (opCode == 0x28);
    bool sh_cond = (opCode == 0x29);
    bool sw_cond = (opCode == 0x2B);

    bool mfco_cond = (opCode == 0x10) & (rsvar == 0x0);
    bool mtco_cond = (opCode == 0x10) & (rsvar == 0x4);

    bool jr_cond = (r_type & (functvar == 0x08));
    bool jalr_cond = (r_type & (functvar == 0x09));
    bool sys_cond = (r_type & (functvar == 0x0C));
    bool break_cond = (r_type & (functvar == 0x0D));
    bool mfhi_cond = (r_type & (functvar == 0x10));
    bool mflo_cond = (r_type & (functvar == 0x12));
    //BYTE || HALF || SYS || MTCO || MFCO || FUNCTOP || UNSIGNED || JUMP || ZERO || MEMTOREG || ALUSRC || BRANCH || MEMREAD || MEMWRITE || REGWRITE || REGDEST    
    *control = 0x0;
    //Set Byte
    *control |= (lb_cond | sb_cond | lbu_cond) ? BYTEM : 0x0;
    //Set Halfword
    *control |= (lh_cond | sh_cond | lhu_cond) ? HALFM : 0x0;
    //Set Sys
    *control |= (sys_cond | break_cond) ? SYSM : 0x0;
    //Set MTCO
    *control |= mtco_cond ? MTCOM : 0x0;
    //Set MFCO
    *control |= mfco_cond ? MFCOM : 0x0;
    //Set Functop
    *control |= (addi_cond | addiu_cond | andi_cond | lui_cond | ori_cond | slti_cond | sltiu_cond | xori_cond) ? FUNCTOPM : 0x0;
    //Set Unsigned
    *control |= (lbu_cond | lhu_cond | sltiu_cond | lui_cond | addiu_cond) ? UNSIGNEDM : 0x0;
    //Set Jump
    *control |= (j_cond | jal_cond | jr_cond | jalr_cond) ? JUMPM : 0x0;
    //Set memtoreg
    *control |= (lw_cond | lh_cond | lhu_cond | lb_cond | lbu_cond) ? MEMTOREGM : 0x0;
    //Set alusrc
    *control |= (lw_cond | lh_cond | lhu_cond | lb_cond | lbu_cond | sb_cond | sh_cond | sw_cond | addi_cond | addiu_cond | andi_cond | lui_cond | ori_cond | slti_cond | sltiu_cond | xori_cond) ? ALUSRCM : 0x0;
    //Set Branch
    *control |= (*branchflags != 0) ? BRANCHM : 0x0;
     //Set Memread
    *control |= (lw_cond | lh_cond | lhu_cond | lb_cond | lbu_cond) ? MEMREADM : 0x0;
    //Set Memwrite
    *control |= (sb_cond | sh_cond | sw_cond) ? MEMWRITEM : 0x0;
    //Set Regwrite
    *control |= (r_type | lw_cond | lh_cond | lhu_cond | lb_cond | lbu_cond | addi_cond | addiu_cond | andi_cond | lui_cond | ori_cond | slti_cond | sltiu_cond | xori_cond | bgezal_cond | bltzal_cond) ? REGWRITEM : 0x0;
    //Set Regdest
    *control |= r_type ? REGDESTM : 0x0;

    //Set ALUOP
    *aluop = 0x0;
    *aluop |= (*branchflags != 0) ? 0x1 : 0x0; 
    *aluop |= (r_type | addi_cond | addiu_cond | andi_cond | lui_cond | ori_cond | slti_cond | sltiu_cond | xori_cond) ? 0x2 : 0x0; 

    bool add_cond = r_type && ((functvar == 0x20) || (functvar == 0x21));
    bool slt_cond = r_type && ((functvar == 0x2A) || (functvar == 0x2B));
    bool sub_cond = r_type && ((functvar == 0x22) || (functvar == 0x23));
    bool sl_cond = r_type && ((functvar == 0x0) || (functvar == 0x4));
    bool sr_cond = r_type && ((functvar == 0x3) || (functvar == 0x7) || (functvar == 0x2) || (functvar == 0x6));
    bool mult_cond = r_type && ((functvar == 0x18) || (functvar == 0x19));
    
    //Subleq Checks
    *executesubleqstart = 0x0;
    *executesubleqstart = 0x0 
                            #if (ARCH == SUBLEQM) | (ARCH == SUBLEQRS) | (ARCH == SUBLEQREV) | (ARCH == SUBLEQB)
                            | ((addi_cond | addiu_cond | (r_type & add_cond)) ? SUBROUT_ADD_START : 0x0) | ((slti_cond | sltiu_cond | (r_type & slt_cond)) ? SUBROUT_SLT_START : 0x0) | ((r_type & sub_cond) ? SUBROUT_SUB_START : 0x0) | ((r_type & sl_cond) ? SUBROUT_SL_START : 0x0)
                            #endif
                            #if (ARCH == SUBLEQM)
                            | ((r_type & sr_cond) ? SUBROUT_SR_START : 0x0)
                            #endif
                            #if (ARCH == SUBLEQRS)
                            | ((r_type & mult_cond) ? SUBROUT_MULT_START: 0x0)
                            #endif
                            #if (ARCH == SUBLEQREV) | (ARCH == SUBLEQB)
                            | ((r_type & mult_cond) ? SUBROUT_MULT_START: 0x0) | ((r_type & sr_cond) ? SUBROUT_SR_START : 0x0)
                            #endif
                            ;
}

void mipsexecution() {
    size_t instrcnt = sizeof(MEM);

    pc = PROG_START << 2;
    //Initializations
    //IF
    uregint_t if_ir=NOOP, if_pc=0;
    //ID
    uregint_t id_ir=NOOP, id_pc=0, id_executesubleqroutine=0, id_target, id_branchaddress=0;
    uint8_t id_opc=0, id_rs=0, id_rt=0, id_rd=0, id_funct=0, id_aluop=0, id_branchflags=0, id_destreg=0;
    uint16_t id_control=0;
    immregint_t id_imm=0;
    regint_t id_regRs=0, id_regRt=0, id_signext=0;
    //EXE
    uregint_t ex_ir=NOOP, ex_pc=0, ex_executesubleqroutine=0;
    uint16_t ex_control=0;
    uint8_t ex_aluop=0, ex_rs=0, ex_rt=0, ex_rd=0, ex_destreg=0, ex_funct=0, ex_aluctrl=0, ex_sa=0;
    regint_t ex_regRs=0, ex_regRt=0, ex_signext=0, ex_op2=0, ex_aluout=0;
    //MEM
    uregint_t mem_ir=0;
    uint16_t mem_control=0;
    uint8_t mem_destreg=0;
    regint_t mem_memdata=0, mem_aluout=0, mem_memout=0, mem_regRs=0, mem_result=0;
    //WB
    uregint_t wb_ir=0;
    regint_t wb_memout=0, wb_aluout=0, wb_result=0;
    uint8_t wb_destreg=0;
    uint16_t wb_control=0;    

    //Subleq
    uregint_t sub_ir = NOOP;
    uint8_t sub_src1reg=0, sub_src2reg=0;
    immregint_t sub_branchval=0;
    regint_t sub_src1val=0, sub_src2val=0, sub_res = 0, sub_aluout=0;

    //Stall, BTB, Rest
    bool stall=false, id_reset=false, ex_reset=false, actualoutcome=false, squashstall=false;
    uregint_t storedbranch = 0;
    uint8_t state = 0x0, nextstate = 0x0;


    while(pc < instrcnt) {
        //stages in every cycle   
        pcnext = incrementPC(pc);

        //Write Back - Stage 5 of pipeline  
        wb_result = ((wb_control & MEMTOREGM) != 0) ? wb_memout : wb_aluout;       
        bool lb_cond = ((wb_ir >> 26) == 0x20);
        bool lbu_cond = ((wb_ir >> 26) == 0x24);
        bool lh_cond = ((wb_ir >> 26) == 0x21);
        bool lhu_cond = ((wb_ir >> 26) == 0x25);
        wb_result = lb_cond ? ((wb_result & 0xFF000000) >> 24) : wb_result;        
        wb_result = lh_cond ? ((wb_result & 0xFFFF0000) >> 16) : wb_result;
        wb_result = lbu_cond ? ((wb_result >> 24) & 0xFF) : wb_result;        
        wb_result = lhu_cond ? ((wb_result >> 16) & 0xFFFF) : wb_result;
        wb_result = ((wb_control & UNSIGNEDM) != 0) ? (uregint_t) wb_result : wb_result;

        store_value(wb_destreg << 2, wb_result, ((wb_control & REGWRITEM) != 0));
        #ifdef MONITOR
        if(LOGLEVEL == 3) {
            printf("Read Values in Write Back Stage:\ndestReg: %d, ALU Output: %u, Memory Output: %u\n", mem_destreg, mem_aluout, mem_memdata);
            controlsignalsprint(mem_control, 0b01000000);
        }
        if(LOGLEVEL >= 2)
            printf("Final Result passed to write to the registers: %u\n", wb_result);
        #endif


        //Memory Access - Stage 4 of pipeline
        //Data forwarding Unit
        mem_aluout = (((mem_control & MEMTOREGM)!=0) && ((wb_control & REGWRITEM)!=0) && (wb_destreg != 0) && (wb_destreg == mem_destreg)) ? wb_result : mem_aluout;
        
        bool sb_cond = ((mem_ir >> 26) == 0x28);
        bool sh_cond = ((mem_ir >> 26) == 0x29);
        mem_aluout = sb_cond ? ((mem_aluout & 0xFF000000) >> 24) : mem_aluout;        
        mem_aluout = sh_cond ? ((mem_aluout & 0xFFFF0000) >> 16) : mem_aluout;             
        mem_memout = get_value((uregint_t) (mem_aluout & 0xFFFF), ((mem_control & MEMREADM) != 0));
        store_value((uregint_t) (mem_aluout & 0xFFFF), mem_memdata, ((mem_control & MEMWRITEM) != 0));
        mem_result = ((mem_control & MEMTOREGM)!=0) ? mem_memout : mem_aluout;
        #ifdef MONITOR
        if(LOGLEVEL == 3) {
            printf("Read Values in Memory Stage:\nALU Output: %u, Memory Output: %u, destReg: %d\n", ex_aluout, ex_regRt, ex_destreg);
            controlsignalsprint(ex_control, 0b00011100);
        }
        if(LOGLEVEL >= 2)
            printf("Written values to MEM - WB Stage Registers:\ndestReg: %d, ALU Output: %u, Memory Output: %u\n", mem_destreg, mem_aluout, mem_memdata);
        #endif


        //Execute - Stage 3 of pipeline        
        ex_funct = (ex_signext & FUNCTM);
        ex_op2 = ((ex_control & ALUSRCM) != 0) ? ex_signext : ex_regRt;

        //Forwarding logic
        ex_regRs = (((wb_control & REGWRITEM)!=0) && (wb_destreg != 0) && (wb_destreg == ex_rs)) ? wb_result : ex_regRs;
        ex_regRs = (((mem_control & REGWRITEM)!=0) && (mem_destreg != 0) && (mem_destreg == ex_rs)) ? mem_result : ex_regRs;

        ex_op2 = (((wb_control & REGWRITEM)!=0) && (wb_destreg != 0) && (wb_destreg == ex_rt)) ? wb_result : ex_op2;
        ex_op2 = (((mem_control & REGWRITEM)!=0) && (mem_destreg != 0) && (mem_destreg == ex_rt)) ? mem_result : ex_op2;

        //Shift left or right with shift amount
        bool sa_sel_cond = ((ex_ir >> 26) == 0x0) && (((ex_ir & FUNCTM) == 0x0) || ((ex_ir & FUNCTM) == 0x3) || ((ex_ir & FUNCTM) == 0x2));
        ex_sa = (ex_ir >> 6) & SHAMTM;
        ex_regRs = sa_sel_cond ? ex_sa : ex_regRs;
        
        ex_aluout = 0;
        bool subleqexec = ex_executesubleqroutine != 0;
        if(!subleqexec) {
            uint8_t functop = ((ex_control & FUNCTOPM) != 0) ? (ex_ir >> 26) : ex_funct;        
            generateAluControl(ex_aluop, functop, &ex_aluctrl);
            ex_aluout = alu(ex_regRs, ex_op2, ex_aluctrl, &ex_control);
        }

        //Subleq execution
        #if (ARCH == SUBLEQREV) | (ARCH == SUBLEQM) | (ARCH == SUBLEQRS) | (ARCH == SUBLEQB)
        #if (ARCH == SUBLEQREV)
        bool isbitreversal = false, isnegative = (ex_op2 < 0);
        if(ex_executesubleqroutine == SUBROUT_SL_START){
            regint_t revshiftamt = ex_regRs, bitrev = ex_op2;
            if(ex_regRs > 16) {
                revshiftamt = 32 - ex_regRs;
                ex_executesubleqroutine = SUBROUT_SR_START;
                isbitreversal = true;
                bitrev = bitreversalop(ex_op2);
            }
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC1) << 2, bitrev, (ex_executesubleqroutine != 0x0));
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC2) << 2, revshiftamt, (ex_executesubleqroutine != 0x0));
        } else if (ex_executesubleqroutine == SUBROUT_SR_START) {
            regint_t revshiftamt = ex_regRs, bitrev = ex_op2;
            if(ex_regRs > 16) {
                revshiftamt = 32 - ex_regRs;
                ex_executesubleqroutine = SUBROUT_SL_START;
                isbitreversal = true;
                bitrev = bitreversalop(ex_op2);
            }
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC1) << 2, bitrev, (ex_executesubleqroutine != 0x0));
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC2) << 2, revshiftamt, (ex_executesubleqroutine != 0x0));
        } else if (ex_executesubleqroutine == SUBROUT_MULT_START) {
            regint_t revop1 = retmax(ex_op2, ex_regRs), revop2 = retmin(ex_op2, ex_regRs);
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC1) << 2, revop1, (ex_executesubleqroutine != 0x0));
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC2) << 2, revop2, (ex_executesubleqroutine != 0x0));
        } else {
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC1) << 2, ex_regRs, (ex_executesubleqroutine != 0x0));
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC2) << 2, ex_op2, (ex_executesubleqroutine != 0x0));
        }
        #else
        if(ex_executesubleqroutine == SUBROUT_SL_START || ex_executesubleqroutine == SUBROUT_SR_START) {
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC1) << 2, ex_op2, (ex_executesubleqroutine != 0x0));
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC2) << 2, ex_regRs, (ex_executesubleqroutine != 0x0));
        } else {
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC1) << 2, ex_regRs, (ex_executesubleqroutine != 0x0));
            store_value((SUBROUT_MEM_OFFSET + SUBROUT_SRC2) << 2, ex_op2, (ex_executesubleqroutine != 0x0));
        } 
        #endif       
        while ((ex_executesubleqroutine < SUBROUT_LAST_INSTR) & (ex_executesubleqroutine != 0x0)) {
            sub_ir = get_value(ex_executesubleqroutine << 2, true);
            sub_src1reg = (sub_ir >> 21) & SRC1REG;
            sub_src2reg = (sub_ir >> 16) & SRC2REG;
            sub_branchval = (sub_ir & BRANCHVAL) >> 2;
            sub_src1val = get_value((sub_src1reg + SUBROUT_MEM_OFFSET) << 2, true);
            sub_src2val = get_value((sub_src2reg + SUBROUT_MEM_OFFSET) << 2, true);
            sub_res = sub_src2val - sub_src1val;
            store_value((sub_src2reg + SUBROUT_MEM_OFFSET) << 2, sub_res, true); 
            #ifdef MONITOR
            printf("Subleq Instruction Executed:\nSRC1: %d,\tSRC2: %d, CURRENT PC: %d, BRANCH: %d\n", sub_src1val, sub_src2val, ex_executesubleqroutine, sub_branchval);
            #endif
            subleqinstrcount++;
            ex_executesubleqroutine = (sub_res <= 0) ? (SUBROUT_FIRST_INSTR + sub_branchval) : (ex_executesubleqroutine + 1); 
        }
        sub_aluout = get_value((SUBROUT_DEST + SUBROUT_MEM_OFFSET) << 2, true);
        if(isbitreversal && !isnegative) {
            sub_aluout = bitreversalop(sub_aluout) & 0xFFFFFFFF;
        } else if (isbitreversal) {
            sub_aluout = bitreversalop(sub_aluout);
        }
            
        #ifdef MONITOR
        printf("Final Subleq Result:\t%d\n", sub_aluout);
        #endif
        #endif        

        ex_aluout = subleqexec ? sub_aluout : ex_aluout;

        bool mfhi_cond = (((ex_ir >> 26) == 0x0) && ((ex_ir & FUNCTM) == 0x10));
        bool mflo_cond = (((ex_ir >> 26) == 0x0) && ((ex_ir & FUNCTM) == 0x12));

        ex_aluout = mfhi_cond ? get_value(HI_LOC << 2, true) : ex_aluout;
        ex_aluout = mflo_cond ? get_value(HI_LOC << 2, true) : ex_aluout;

        #ifdef MONITOR
        if(LOGLEVEL == 3) {
            printf("Read Values in Execute Stage:\nregRs: %u, regRt: %u, signExt: %u, aluOp: %d\n", id_regRs, id_regRt, id_signext, id_aluop);
            controlsignalsprint(ex_control, 0b10100001);
        }
        if(LOGLEVEL >= 2) 
            printf("Written values to EX - MEM Stage Registers:\nALU Output: %u, Memory Output: %u, destReg: %d\n", ex_aluout, ex_regRt, ex_destreg);
        #endif


        //Instruction Decode - Stage 2 of pipeline
        id_opc = id_ir >> 26;
        id_rs = (id_ir >> 21) & RSM;
        id_rt = (id_ir >> 16) & RTM;
        id_rd = (id_ir >> 11) & RDM;
        id_funct = id_ir & FUNCTM;
        id_imm = id_ir & IMMEDM;
        id_target = id_ir & JUMPFM;
        generateControlSignals(id_ir, &id_control, &id_aluop, &id_branchflags, &id_executesubleqroutine);

        //Hazard Detection Unit
        stall = (((id_rs == ex_destreg) || (id_rt == ex_destreg)) && ((ex_control & MEMTOREGM) != 0)) || ((id_control & JUMPM) != 0) || 
                (((id_control & BRANCHM) != 0) && ((mem_control & REGWRITEM) != 0) && ((id_rs == mem_destreg) || (id_rt == mem_destreg)));
                
        squashstall = ((((id_control & BRANCHM) != 0)) && ((ex_control & REGWRITEM) != 0) && ((id_rs == ex_destreg) || (id_rt == ex_destreg)));
                

        id_control = (id_ir == NOOP) ? 0x0 : id_control;   
        id_destreg = ((id_control & REGDESTM) != 0) ? id_rd : id_rt;      
        id_regRs = get_value(id_rs << 2, true);
        id_regRt = get_value(id_rt << 2, true);

        //Data Forwarding Unit
        id_regRs = (((mem_control & REGWRITEM)!=0) && (id_rs != 0) && (mem_destreg == id_rs)) ? mem_aluout : id_regRs;
        id_regRt = (((mem_control & REGWRITEM)!=0) && (id_rt != 0) && (mem_destreg == id_rt)) ? mem_aluout : id_regRt;

        id_signext = signExtn(id_imm);

        bool id_j_cond = (id_opc == 0x02);
        bool id_jal_cond = (id_opc == 0x03);
        bool id_jr_cond = ((id_opc == 0x00) & (id_funct == 0x08));
        bool id_jalr_cond = ((id_opc == 0x00) & (id_funct == 0x09));
        bool sys_cond = ((id_opc == 0x00) & (id_funct == 0x0C));
        bool break_cond = ((id_opc == 0x00) & (id_funct == 0x0D));

        //PC Branch Calculations
        id_branchaddress = (uregint_t) addBranchAddr(id_pc, id_imm << 2);
        //BNE || BLTZAL || BLTZ || BLEZ || BGTZ || BGEZAL || BGEZ || BEQ
        //Linking return address
        store_value(RETADDR_LOC << 2, id_pc, (((id_branchflags & BLTZALM) != 0) | ((id_branchflags & BGEZALM) != 0)));

        //Branch or not
        actualoutcome = (!squashstall) && ((((id_branchflags & BNEM) != 0) & (id_regRs != id_regRt)) | 
                    (((id_branchflags & BLTZALM) != 0) & (id_regRs < 0)) | 
                    (((id_branchflags & BLTZM) != 0) & (id_regRs < 0)) |
                    (((id_branchflags & BLEZM) != 0) & (id_regRs <= 0)) |
                    (((id_branchflags & BGTZM) != 0) & (id_regRs > 0)) |
                    (((id_branchflags & BGEZALM) != 0) & (id_regRs >= 0)) | 
                    (((id_branchflags & BGEZM) != 0) & (id_regRs >= 0)) |
                    (((id_branchflags & BEQM) != 0) & (id_regRs == id_regRt)));

        
        pcnext = (stall || squashstall) ? pc : pcnext;
        #if BENCHMARK == 0 || BENCHMARK == 15
        pcnext = actualoutcome ? id_branchaddress : pcnext;
        #else
        pcnext = actualoutcome ? (id_branchaddress - 4) : pcnext;
        #endif      

        //Jump Calculations
        if(id_j_cond)
            pcnext =  ((pcnext & JUMP_RPCM) | id_target << 2) - 4;
        else if(id_jr_cond)
            pcnext = id_regRs - 4;
        else if(id_jal_cond) {
            store_value(RETADDR_LOC << 2, id_pc, true);
            pcnext = (id_target << 2) + 4;
        } else if(id_jalr_cond) {
            store_value(id_rd << 2, id_pc, true);
            pcnext = id_regRs + 4;
        }  
        
        if(sys_cond) {  
            //Syscalls
            uregint_t vi_val = get_value(V0_LOC << 2, sys_cond);
            //Forwarding vi value
            vi_val = (((mem_control & REGWRITEM)!=0) && (mem_destreg == V0_LOC)) ? mem_aluout : vi_val;
            vi_val = (((ex_control & REGWRITEM)!=0) && (ex_destreg == V0_LOC)) ? mem_aluout : vi_val; 

            if(vi_val == 10) break;
        #ifdef SYSCALLS
            regint_t a0val1 = 0x0;            
            uregint_t a0val = 0x0, charstr = 0x0; 
            uint8_t char1 = 0x0, char2 = 0x0, char3 = 0x0, char4 = 0x0;         
            switch (vi_val)
            {
            case 1: 
                a0val1 = (regint_t) get_value(A0_LOC << 2, true);
                //Forwarding a0 value
                a0val1 = (((mem_control & REGWRITEM)!=0) && (mem_destreg == A0_LOC)) ? mem_aluout : a0val1;
                a0val1 = (((ex_control & REGWRITEM)!=0) && (ex_destreg == A0_LOC)) ? mem_aluout : a0val1;
                printf("%d", a0val1);
                break;
            case 4: 
                a0val = get_value(A0_LOC << 2, true);
                //Forwarding a0 value
                a0val = (((mem_control & REGWRITEM)!=0) && (mem_destreg == A0_LOC)) ? mem_aluout : a0val;
                a0val = (((ex_control & REGWRITEM)!=0) && (ex_destreg == A0_LOC)) ? mem_aluout : a0val;
                while(1) {
                    charstr = get_value(a0val << 2, true);
                    char1 = charstr & 0xFF000000, char2 = charstr & 0xFF0000, char3 = charstr & 0xFF00, char4 = charstr & 0xFF;
                    if(char1 == 0) break;
                    printf("%c", char1);
                    if(char2 == 0) break;
                    printf("%c", char2);
                    if(char3 == 0) break;
                    printf("%c", char3);
                    if(char4 == 0) break;
                    printf("%c", char4);
                }
                break;
            default:
                break;
            }
        #endif
        }

        #ifdef MONITOR
        if(LOGLEVEL == 3) {
            printf("Read Values in Instruction Decode Stage:\nprogram counter: %u\tinstruction: %u\n", if_pc, if_ir);
            controlsignalsprint(id_control, 0b01111111);
        }
        if(LOGLEVEL >= 2)
            printf("Written values to ID - EX Stage Registers:\nprogram counter: %u, instruction: %x, rt: %d, rd: %d, regRs: %u, regRt: %u, aluop: %d\n", id_pc, id_ir, id_rt, id_rd, id_regRs, id_regRt, id_aluop);
        #endif 


        //Instruction Fetch - Stage 1 of pipeline
        if_ir = get_value(pc, true);
        if_pc = pc;
        #ifdef MONITOR
        if(LOGLEVEL == 3) {
            printf("Read Values in Instruction Fetch Stage:\nbranch address: %d\n", id_branchaddress);
            controlsignalsprint(ex_control, 0b10010000);
        }
        if(LOGLEVEL >=2)    
            printf("Written values to IF - ID Stage Registers:\nprogram counter: %d, instruction: %x\n", if_pc, if_ir);
        #endif
        #ifdef PROFILE
        instructioninfo(if_ir);
        #endif
        

        //update the stage registers
        //MEM WB Update
        wb_ir = mem_ir; wb_control = (mem_ir == NOOP) ? 0x0 : mem_control; wb_aluout = (mem_ir == NOOP) ? 0x0 : mem_aluout; wb_destreg = (mem_ir == NOOP) ? 0x0 : mem_destreg; wb_memout = (mem_ir == NOOP) ? 0x0 : mem_memout;
        //EXE MEM
        mem_ir = ex_ir; mem_control = (ex_ir == NOOP) ? 0x0 : ex_control; mem_destreg = (ex_ir == NOOP) ? 0x0 : ex_destreg; mem_memdata = (ex_ir == NOOP) ? 0x0 : ex_regRt; mem_aluout = (ex_ir == NOOP) ? 0x0 : ex_aluout; mem_regRs = (ex_ir == NOOP) ? 0x0 : ex_regRs;
        //ID_EXE Update
        ex_ir = id_ir; ex_pc = id_pc; ex_regRs = (id_ir == NOOP) ? 0x0 : id_regRs; ex_regRt = (id_ir == NOOP) ? 0x0 : id_regRt; ex_signext = (id_ir == NOOP) ? 0x0 : id_signext; ex_control = (id_ir == NOOP) ? 0x0 : id_control; ex_aluop = (id_ir == NOOP) ? 0x0 : id_aluop; ex_rs = (id_ir == NOOP) ? 0x0 : id_rs; ex_rt = (id_ir == NOOP) ? 0x0 : id_rt; ex_rd = (id_ir == NOOP) ? 0x0 : id_rd; ex_executesubleqroutine = (id_ir == NOOP) ? 0x0 : id_executesubleqroutine; ex_destreg = (id_ir == NOOP) ? 0x0 : id_destreg;
        //IF-ID Update
        id_ir = stall ? NOOP : if_ir; id_pc = stall ? (PROG_START << 2) : pcnext;
        id_ir = squashstall ? ex_ir : id_ir; id_pc = squashstall ? ex_pc : id_pc;
        
        pc = pcnext;
    }
}

int main(void) {
    mipsexecution();

    #ifdef PROFILE
    uregint_t i;
    printf("========\nREG FILE\n========\n");
    for (i = 0; i < 32; i++){
        printf("%d\t", get_value(i << 2, true));
        if (((i+1) % 4) == 0)
        printf("\n");
    }

    printf("\n=====================\nInstruction Profiling\n=====================\n");
    printf("Addition (add, addu, addi, addiu): %d\n", addition);
    printf("Subtraction (sub, subu): %d\n", subtraction);
    printf("Shift (sll, sllv, srl, srlv, sra, srav): %d\n", shift);
    printf("Logic (and, andi, or, ori, xor, xori, nor): %d\n", logic);
    printf("Multiplication (mult, multu, mflo, mfhi, mtlo, mthi): %d\n", multiplication);
    printf("Set-less-than (slt, slti. sltu, sltui ...): %d\n", slt_u);
    printf("Jump (j, jal, ...): %d\n", jump);
    printf("Load (lw, lh, lhu, lb, lbu): %d\n", load);
    printf("Store (sw, sh, sb): %d\n", store);
    printf("Conditional branch (beq, beqz, ble, ....): %d\n", branch);
    printf("Total Number of Subleq Instructions Executed: %d\n", subleqinstrcount);
    
    #if BENCHMARK == 0
	for (int i = 0; i < 32; i++) {
        int data = get_value(8768-(4*i), true);
		printf("%d, ", data);
	}
    #endif

    #if BENCHMARK == 15
    for (int i = 0; i < 10; i++) {
        int data = get_value(16384+(4*i), true);
		printf("%d, ", data);
	}    
    #endif
    #endif
    return 0;
}