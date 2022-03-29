// This code is mostly complete. You need to just fill in the lines where it says 
// "... your code goes here"
module proc(DIN, Resetn, Clock, Run, Done);
    input [15:0] DIN;//each instruction is 16 bit//
    input Resetn, Clock, Run;
    output Done;

    wire [0:7] R_in; // r0, ..., r7 register enables
    reg [15:0] Sum;
    reg rX_in, IR_in, Done, A_in, G_in, AddSub;
    reg [2:0] Tstep_Q, Tstep_D;
    reg [15:0] BusWires;
    reg [3:0] Select; // BusWires multiplexer select input
    wire [2:0] III, rX, rY; // instruction opcode and register operands
    wire [15:0] A;
	 wire [15:0] r [7:0];    //Define 8 16-bit register arrays
    wire [15:0] G;
    wire [15:0] IR;
    wire Imm;
   
    assign III = IR[15:13];
    assign Imm = IR[12];
    assign rX = IR[11:9]; //[0：15]//
    assign rY = IR[2:0];
    dec3to8 decX (rX_in, rX, R_in); // produce r0 - r7 register enables

    parameter T0 = 3'b000, T1 = 3'b001, T2 = 3'b010, T3 = 3'b011;

    // Control FSM state table
    always @(Tstep_Q, Run, Done) begin
        case (Tstep_Q)
            T0: // data is loaded into IR in this time step
                if (~Run) Tstep_D = T0;
                else Tstep_D = T1;
            T1: if (Done) Tstep_D = T0;
                else Tstep_D = T2; //not done//
            T2: if (Done) Tstep_D = T0;
                else Tstep_D = T3; //not done//
            T3: // instructions end after this time step
                Tstep_D = T0;
        endcase
    end

    /* OPCODE format: III M XXX DDDDDDDDD, where 
    *     III = instruction, M = Immediate, XXX = rX
    *     If M = 0, DDDDDDDDD = 000000YYY = rY
    *     If M = 1, DDDDDDDDD = #D is the immediate operand 
    *
    *  III M  Instruction   Description
    *  --- -  -----------   -----------
    *  000 0: mv   rX,rY    rX <- rY
    *  000 1: mv   rX,#D    rX <- D (sign extended)
    *  001 1: mvt  rX,#D    rX <- D << 8
    *  010 0: add  rX,rY    rX <- rX + rY
    *  010 1: add  rX,#D    rX <- rX + D
    *  011 0: sub  rX,rY    rX <- rX - rY
    *  011 1: sub  rX,#D    rX <- rX - D */
    parameter mv = 3'b000, mvt = 3'b001, add = 3'b010, sub = 3'b011;
    // selectors for the BusWires multiplexer
    parameter R0_SELECT = 4'b0000, R1_SELECT = 4'b0001, R2_SELECT = 4'b0010, 
        R3_SELECT = 4'b0011, R4_SELECT = 4'b0100, R5_SELECT = 4'b0101, R6_SELECT = 4'b0110, 
        R7_SELECT = 4'b0111, G_SELECT = 4'b1000, 
        SGN_IR8_0_SELECT /* signed-extended immediate data */ = 4'b1001, 
        IR7_0_0_0_SELECT /* immediate data << 8 */ = 4'b1010;
        
    // Control FSM outputs
    always @(*) begin
        // default values for control signals
        rX_in = 1'b0; A_in = 1'b0; G_in = 1'b0; AddSub = 1'b0; IR_in = 1'b0; Select = 4'bxxxx;
        Done = 1'b0; //default values//
        case (Tstep_Q)
            T0: // store instruction on DIN in IR  
                IR_in = 1'b1;
            T1: // define signals in T1 
                case (III)      // see table 2 - all//
                    mv: begin
						if (!Imm) begin            //  rX <- rY
						Select = rY;
						rX_in = 1'b1;
						Done = 1'b1;
						end
						else begin
						Select = SGN_IR8_0_SELECT ;   // rX <- D (sign extended)
						rX_in = 1'b1;          //enable rX
						Done = 1'b1;
						end						
                    end
                    mvt: begin                   // rX <- D << 8
						Select = IR7_0_0_0_SELECT;
						rX_in = 1'b1;
						Done = 1'b1;
                    end
                    add, sub: begin          // Load the value of the rX register into the A register
						Select = rX;
						A_in = 1'b1;
                    end
                    default: ;
                endcase
            T2: // define signals T2
                case (III)         //add, sub//
                    add: begin
                   // Load the value of A+rY or [A+(signed extended)D] into G 
						if (!Imm) begin
						Select = rY;
						G_in = 1'b1;
						end
						else begin
						Select = SGN_IR8_0_SELECT ;
						G_in = 1'b1;
						end
                    end
                    sub: begin
                  // Load the value of A-rY or [A-(signed extended)D] into G register
						if (!Imm) begin
						Select = rY;
						AddSub = 1'b1;
						G_in = 1'b1;
						end
						else begin
						Select = SGN_IR8_0_SELECT;
						AddSub = 1'b1;
						G_in = 1'b1;
						end
                    end
                    default: ; 
                endcase
            T3: // define T3
                case (III)
                    add, sub: begin           // Load the value in G into the rX register
						Select = G_SELECT;
						rX_in = 1'b1;
						Done = 1'b1;
                    end
                    default: ;
                endcase
            default: ;
        endcase
    end   
   
    // Control FSM flip-flops
    always @(posedge Clock)
        if (!Resetn)
            Tstep_Q <= T0;
        else
            Tstep_Q <= Tstep_D;
				
    //Load the value of one of the registers into the buswires
    regn reg_0 (BusWires, Resetn, R_in[0], Clock, r[0]);
    regn reg_1 (BusWires, Resetn, R_in[1], Clock, r[1]);
    regn reg_2 (BusWires, Resetn, R_in[2], Clock, r[2]);
    regn reg_3 (BusWires, Resetn, R_in[3], Clock, r[3]);
    regn reg_4 (BusWires, Resetn, R_in[4], Clock, r[4]);
    regn reg_5 (BusWires, Resetn, R_in[5], Clock, r[5]);
    regn reg_6 (BusWires, Resetn, R_in[6], Clock, r[6]);
    regn reg_7 (BusWires, Resetn, R_in[7], Clock, r[7]);

    regn reg_A (BusWires, Resetn, A_in, Clock, A);
    regn reg_IR (DIN, Resetn, IR_in, Clock, IR);

    // alu
    always @(*)
        if (!AddSub)
            Sum = A + BusWires;
        else
            Sum = A + ~BusWires + 16'b1;

    regn reg_G (Sum, Resetn, G_in, Clock, G);

    // define the internal processor bus
    always @(*)
        case (Select)
            R0_SELECT: BusWires = r[0];
            R1_SELECT: BusWires = r[1];
            R2_SELECT: BusWires = r[2];
            R3_SELECT: BusWires = r[3];
            R4_SELECT: BusWires = r[4];
            R5_SELECT: BusWires = r[5];
            R6_SELECT: BusWires = r[6];
            R7_SELECT: BusWires = r[7];
            G_SELECT: BusWires = G;
            SGN_IR8_0_SELECT: BusWires = {{7{IR[8]}}, IR[8:0]};   // sign extended
            IR7_0_0_0_SELECT: BusWires = {IR[7:0], 8'b00000000};
            default: BusWires = 16'bxxxxxxxxxxxxxxxx;
        endcase
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

module regn(R, Resetn, Rin, Clock, Q);
    parameter n = 16;
    input [n-1:0] R;
    input Resetn, Rin, Clock;
    output [n-1:0] Q;
    reg [n-1:0] Q;

    always @(posedge Clock)
        if (!Resetn)
            Q <= 0;
        else if (Rin)
            Q <= R;
endmodule
