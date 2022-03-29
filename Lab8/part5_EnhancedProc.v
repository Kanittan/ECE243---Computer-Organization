module proc(DIN, Resetn, Clock, Run, DOUT, ADDR, W);
    input [15:0] DIN;
    input Resetn, Clock, Run;
    output wire [15:0] DOUT;
    output wire [15:0] ADDR;
    output wire W;

    wire [0:7] R_in; // r0, ..., r7 register enables
    reg rX_in, IR_in, ADDR_in, Done, DOUT_in, A_in, G_in, AddSub, ALU_and,C_in;
    reg [2:0] Tstep_Q, Tstep_D;
    reg [15:0] BusWires;
    reg [3:0] Select; // BusWires selector
    reg [15:0] Sum;
	 //wire [15:0] Sum ;
	reg  cout;
	//wire cout;
	wire z,n;
	wire c;
	 //reg c;
    wire [2:0] III, rX, rY; // instruction opcode and register operands
    wire [15:0] r0, r1, r2, r3, r4, r5, r6, pc, A;
    wire [15:0] G;
    wire [15:0] IR;
    reg pc_incr;    // used to increment the pc
    reg pc_in;      // used to load the pc
    reg W_D;        // used for write signal
    wire Imm;
   
    assign III = IR[15:13];
    assign Imm = IR[12];
    assign rX = IR[11:9];
    assign rY = IR[2:0];
    dec3to8 decX (rX_in, rX, R_in); // produce r0 - r7 register enables

    parameter T0 = 3'b000, T1 = 3'b001, T2 = 3'b010, T3 = 3'b011, T4 = 3'b100, T5 = 3'b101;

    // Control FSM state table
    always @(Tstep_Q, Run, Done)
        case (Tstep_Q)
            T0: // instruction fetch
                if (~Run) Tstep_D = T0;
                else Tstep_D = T1;
            T1: // wait cycle for synchronous memory
                Tstep_D = T2;
            T2: // this time step stores the instruction word in IR
                Tstep_D = T3;
            T3: if (Done) Tstep_D = T0;
                else Tstep_D = T4;
            T4: if (Done) Tstep_D = T0;
                else Tstep_D = T5;
            T5: // instructions end after this time step
                Tstep_D = T0;
            default: Tstep_D = 3'bxxx;
        endcase

    /* OPCODE format: III M XXX DDDDDDDDD, where 
    *     III = instruction, M = Immediate, XXX = rX. If M = 0, DDDDDDDDD = 000000YYY = rY
    *     If M = 1, DDDDDDDDD = #D is the immediate operand 
    *
    *  III M  Instruction   Description
    *  --- -  -----------   -----------
    *  000 0: mv   rX,rY    rX <- rY
    *  000 1: mv   rX,#D    rX <- D (sign extended)
    *  001 1: mvt  rX,#D    rX <- D << 8
	 *  001 0: bcon b{cond} LABEL if(cond) pc <- LABEL
    *  010 0: add  rX,rY    rX <- rX + rY
    *  010 1: add  rX,#D    rX <- rX + D
    *  011 0: sub  rX,rY    rX <- rX - rY
    *  011 1: sub  rX,#D    rX <- rX - D
    *  100 0: ld   rX,[rY]  rX <- [rY]
    *  101 0: st   rX,[rY]  [rY] <- rX
    *  110 0: and  rX,rY    rX <- rX & rY
    *  110 1: and  rX,#D    rX <- rX & D */
    parameter mv = 3'b000, mvt_bcon = 3'b001, add = 3'b010, sub = 3'b011, ld = 3'b100, st = 3'b101,
	     and_ = 3'b110;
    // selectors for the BusWires multiplexer
    parameter R0_SELECT = 4'b0000, R1_SELECT = 4'b0001, R2_SELECT = 4'b0010, 
        R3_SELECT = 4'b0011, R4_SELECT = 4'b0100, R5_SELECT = 4'b0101, R6_SELECT = 4'b0110, 
        PC_SELECT = 4'b0111, G_SELECT = 4'b1000, 
        SGN_IR8_0_SELECT /* signed-extended immediate data */ = 4'b1001, 
        IR7_0_0_0_SELECT /* immediate data << 8 */ = 4'b1010,
        DIN_SELECT /* data-in from memory */ = 4'b1011;
    // Control FSM outputs
   always @(*) begin
        // default values for control signals
        rX_in = 1'b0; A_in = 1'b0; G_in = 1'b0; IR_in = 1'b0; DOUT_in = 1'b0; ADDR_in = 1'b0; 
        Select = 4'bxxxx; AddSub = 1'b0; ALU_and = 1'b0; W_D = 1'b0; Done = 1'b0;
        pc_in = R_in[7] /* default pc enable */; pc_incr = 1'b0; C_in = 1'b0;
 
        case (Tstep_Q)
            T0: begin // fetch the instruction
                Select = PC_SELECT;  // put pc onto the internal bus
                ADDR_in = 1'b1;
                pc_incr = Run; // to increment pc
            end
            T1: // wait cycle for synchronous memory
                ;
            T2: // store instruction on DIN in IR 
                IR_in = 1'b1;
            T3: // define signals in T3
                case (III)
                    mv: begin
                        if (!Imm) Select = rY;          // mv rX, rY
                        else Select = SGN_IR8_0_SELECT; // mv rX, #D
                        rX_in = 1'b1;                   // enable the rX register
                        Done = 1'b1;
                    end
                    mvt_bcon: begin	
						if(Imm) begin
						Select = IR7_0_0_0_SELECT;       //mvt rX,#D
						rX_in = 1'b1;					// enable the rX register
						Done = 1'b1;					//output Done signal
						end
						else begin
						/*
						case (rX)
						000: begin                      //always branch
						Select = PC_SELECT;
						A_in = 1'b1 ;
						end
						001: begin
						if (z==1) begin                 //branch if the alu result is equal to 0
						Select = PC_SELECT;
						A_in = 1'b1 ;
						end						
						end
						010: begin
						if (!z) begin                 //branch if the alu result is not equal to 0
						Select = PC_SELECT;
						A_in = 1'b1 ;
						end						
						end	
						011: begin
						if (!c) begin                 //branch if carry clear
						Select = PC_SELECT;
						A_in = 1'b1 ;
						end						
						end	
						100: begin
						if (c==1) begin                 //branch if carry set
						Select = PC_SELECT;
						A_in = 1'b1 ;
						end						
						end	
						101: begin
						if (!n) begin                 //branch if the alu result is positive
						Select = PC_SELECT;
						A_in = 1'b1 ;
						end						
						end						
						110: begin
						if (n==1) begin                 //branch if the alu result is negative
						Select = PC_SELECT;
						A_in = 1'b1 ;
						end						
						end
						endcase
						*/
						if((rX==3'b000)||((rX==3'b001)&&(z==1))||((rX==3'b010)&&(z==0))||((rX==3'b011)&&(c==0))||((rX==3'b100)&&(c==1))||((rX==3'b101)&&(n==0))||((rX==3'b110)&&(n==1))) begin
						Select = PC_SELECT;
						A_in = 1'b1 ;	
						end
						else begin
						Done = 1'b1;                     //If none of the conditions are met,do nothing and output Done signal
						end
						end
                    end
                    add, sub, and_: begin				//Load the value of the rX register into the A register
						Select = rX ;
						A_in = 1'b1 ;						
                    end
                    ld, st: begin
                  Select = rY;                     //Load the content of the rY register onto the internal Bus
						ADDR_in = 1'b1;                   //Load the Address to the ADDR port of external memory
                    end
                    default: ;
                endcase
            T4: // define signals T2
                case (III)
                    add: begin
                   // Load the value of A+rY or [A+(signed extended)D] into G 
						if (!Imm) Select = rY ;
						else Select = SGN_IR8_0_SELECT ;
						G_in = 1'b1 ;						//Load the value of sum into G register
						C_in = 1'b1 ;						//Load the value of cout into C register
                    end
                    sub: begin
                   // Load the value of A-rY or [A-(signed extended)D] into G register
						if (!Imm) Select = rY ;
						else Select = SGN_IR8_0_SELECT ;
						AddSub = 1'b1 ;
						G_in = 1'b1 ;						//Load the value of sum into G register
						C_in = 1'b1 ;						//Load the value of cout into C register
                    end
                    and_: begin
                   // Load the value of A&rY or [A&(signed extended)D] into G register
						if (!Imm) Select = rY ;
						else Select = SGN_IR8_0_SELECT ;
						G_in = 1'b1 ;                      //Load the value of sum into G register
						ALU_and = 1'b1;
                    end
                    ld: // wait cycle for synchronous memory
                        ;
                    st: begin
                  Select = rX;                     //Load the content of the rX register onto the internal Bus
						DOUT_in = 1'b1;					 //Load the content of rX to the DATA port of external memory
						W_D = 1'b1;                      //enable write operation
                    end
					mvt_bcon: begin
					    // Load the value of pc+D into G 
						if(!Imm) begin
						Select = SGN_IR8_0_SELECT ;
						G_in = 1'b1 ;						//Load the value of sum into G register
						end
					end
                    default: ; 
                endcase
            T5: // define T3
                case (III)
                    add, sub, and_: begin
						Select = G_SELECT ;           // Load the value in G into the rX register
						rX_in = 1'b1;				   // enable the rX register
						Done = 1'b1;				   //output Done signal
                    end
					mvt_bcon: begin
						if(!Imm) begin
						Select = G_SELECT ;           // Load the value in G onto the Bus
						pc_in = 1'b1;				   // enable the pc, load LABEL into pc 
						Done = 1'b1;				   // output Done signal
						end
					end
                    ld: begin
                  Select = DIN_SELECT;           //Load the data read from external memory onto the internal Bus
						rX_in = 1'b1;                   // enable the rX register
						Done = 1'b1;					//output Done signal						
                    end
                    st: begin                           // wait cycle for synhronous memory
						Done = 1'b1;					//output Done signal
						end
                    default: ;
                endcase
            default: ;
        endcase 
    end   
  
    // Control FSM flip-flops
    always @(posedge Clock)
        if (!Resetn)
            Tstep_Q = T0;
        else
            Tstep_Q = Tstep_D;   
   
    regn reg_0 (BusWires, Resetn, R_in[0], Clock, r0);
    regn reg_1 (BusWires, Resetn, R_in[1], Clock, r1);
    regn reg_2 (BusWires, Resetn, R_in[2], Clock, r2);
    regn reg_3 (BusWires, Resetn, R_in[3], Clock, r3);
    regn reg_4 (BusWires, Resetn, R_in[4], Clock, r4);
    regn reg_5 (BusWires, Resetn, R_in[5], Clock, r5);
    regn reg_6 (BusWires, Resetn, R_in[6], Clock, r6);

    // r7 is program counter
    // module pc_count(R, Resetn, Clock, E, L, Q);
    pc_count reg_pc (BusWires, Resetn, Clock, pc_incr, pc_in, pc);

    regn reg_A (BusWires, Resetn, A_in, Clock, A);
    regn reg_DOUT (BusWires, Resetn, DOUT_in, Clock, DOUT);
    regn reg_ADDR (BusWires, Resetn, ADDR_in, Clock, ADDR);
    regn reg_IR (DIN, Resetn, IR_in, Clock, IR);

    flipflop reg_W (W_D, Resetn, Clock, W);
    
    // alu
    
	always @(*)
	begin
        if (!ALU_and)
            if (!AddSub) begin
                if( ({1'b0,A} + {1'b0,BusWires}) > 16'hffff) cout = 1'b1; //save the Carry in cout
						else cout =1'b0;
				    //{cout,Sum} = {1'b0,A} + {1'b0,BusWires};     
						Sum = A + BusWires;
						end
            else begin
				if(A < BusWires) cout = 1'b1;       //Borrow is generated if the meiosis is greater than the minuend
				else cout = 1'b0;			
                Sum = A + ~BusWires + 16'b1;
				end
		  else begin
            Sum = A & BusWires;
			end
	end
	
	/*
	assign cout = (ALU_and == 1) ? 0 : ((AddSub == 1) ? ((A < BusWires) ? 1:0):(((A+BusWires)>16'd255)? 1:0));
	assign Sum = (ALU_and == 1) ? (A & BusWires) : ((AddSub == 1) ? (A + ~BusWires + 16'b1):(A + BusWires));
	*/
	
    regn reg_G (Sum, Resetn, G_in, Clock, G);
	 regn_c regc (cout, Resetn,C_in, Clock, c);  // c<-cout, The c flag should reflect the carry-out from the ALU; this flag should be 1 when an add instruction generates a carry-out, or when a sub operation does not generate a borrow.
	
	// define flag z,c,n
	assign z = (G == 15'b0) ? 1:0 ;    // The z flag should have the value 1 when the ALU generates a result of zero
    //assign c = cout;                   
	assign n = (G[15] == 1'b1) ? 1:0 ; // the n flag should be 1 when the ALUgenerates a result that is negative, meaning that the most-significant bit (the sign bit) is 1
		
    // define the internal processor bus
    always @(*)
        case (Select)
            R0_SELECT: BusWires = r0;
            R1_SELECT: BusWires = r1;
            R2_SELECT: BusWires = r2;
            R3_SELECT: BusWires = r3;
            R4_SELECT: BusWires = r4;
            R5_SELECT: BusWires = r5;
            R6_SELECT: BusWires = r6;
            PC_SELECT: BusWires = pc;
            G_SELECT: BusWires = G;
            SGN_IR8_0_SELECT: BusWires = {{7{IR[8]}}, IR[8:0]}; // sign extended
            IR7_0_0_0_SELECT: BusWires = {IR[7:0], 8'b0};
            DIN_SELECT: BusWires = DIN;
            default: BusWires = 16'bx;
        endcase
endmodule

module pc_count(R, Resetn, Clock, E, L, Q);
    input [15:0] R;
    input Resetn, Clock, E, L;
    output [15:0] Q;
    reg [15:0] Q;
   
    always @(posedge Clock)
        if (!Resetn)
            Q = 16'b0;
        else if (L)
            Q = R;
        else if (E)
            Q = Q + 1'b1;
endmodule

module dec3to8(E, W, Y);
    input E; // enable
    input [2:0] W;
    output [0:7] Y;
    reg [0:7] Y;
   
    always @(*)
        if (E == 0)
            Y = 8'b00000000;
        else
            case (W)
                3'b000: Y = 8'b10000000;
                3'b001: Y = 8'b01000000;
                3'b010: Y = 8'b00100000;
                3'b011: Y = 8'b00010000;
                3'b100: Y = 8'b00001000;
                3'b101: Y = 8'b00000100;
                3'b110: Y = 8'b00000010;
                3'b111: Y = 8'b00000001;
            endcase
endmodule

module regn(R, Resetn, E, Clock, Q);
    parameter n = 16;
    input [n-1:0] R;
    input Resetn, E, Clock;
    output [n-1:0] Q;
    reg [n-1:0] Q;

    always @(posedge Clock)
        if (!Resetn)
            Q = 0;
        else if (E)
            Q = R;
endmodule

// 1 bit register for flag c
module regn_c(R, Resetn, E, Clock, Q);
    input R;
    input Resetn, E, Clock;
    output Q;
    reg Q;

    always @(posedge Clock)
        if (!Resetn)
            Q = 0;
        else if (E)
            Q = R;
endmodule
