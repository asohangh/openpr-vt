`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: CCMLAB, Virginia Tech
// Engineer: Tony Frangieh
// 
// Create Date:    15:13:33 04/09/2011 
// Design Name: 
// Module Name:    iic_controller 
// Project Name: simple_filters
// Target Devices: 
// Tool versions: 
// Description: Configures video input/output chips through iic
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module iic_controller(clock, reset, sda, scl);
  input  clock;
  input  reset;
  inout  sda;
  inout  scl;

  parameter clk_rate_mhz         = 200,
            sck_period_us        = 30,
            transition_cycle     = (clk_rate_mhz * sck_period_us) / 2,
            transition_cycle_msb = 11;

  localparam idle         = 3'd0,
             init         = 3'd1,
             start        = 3'd2,
             clk_fall     = 3'd3,
             setup        = 3'd4,
             clk_rise     = 3'd5,
             wait_state   = 3'd6,
             start_bit    = 1'b1,
             slave_addr   = 7'b1110110,
             slave_addr_0 = 7'h4c,
             ack          = 1'b1,
             write        = 1'b0,

             reg_addr0   = 8'h49,
             data0       = 8'hc0,

             reg_addr1   = 8'h21,
             data1       = 8'h09,

             reg_addr2   = 8'h33,
             data2a      = 8'h06,

             data2b      = 8'h08,
             reg_addr3   = 8'h34,

             data3a      = 8'h26,
             data3b      = 8'h16,

             reg_addr4   = 8'h36,
             data4a      = 8'ha0,
             data4b      = 8'h60,

             // input and power control // 9980
             reg_addr_0  = 8'h1e,
             data_0      = 8'ha4,

             // output_control {[7:5]=output_mode, [4]=primary_out_en,[3]=secondary_out_en,[2:1]=drive_strength,[0]=clk_inv}   // 9980
             reg_addr_1  = 8'h1f,
             data_1      = 8'h14,

             // output select 2
             reg_addr_2  = 8'h20,
             data_2      = 8'h01,

             // red gain
             reg_addr_3  = 8'h05,
             data_3      = 8'h40,

             reg_addr_4  = 8'h06,
             data_4      = 8'h00,

             // green gain
             reg_addr_5  = 8'h07,
             data_5      = 8'h40,

             reg_addr_6  = 8'h08,
             data_6      = 8'h00,

             // blue gain
             reg_addr_7  = 8'h09,
             data_7      = 8'h40,

             reg_addr_8  = 8'h0a,
             data_8      = 8'h00,

             // clamp and offset (auto-offset every 64 clamps)
             reg_addr_9  = 8'h1b,
             data_9      = 8'h33,

             // red offset
             reg_addr_10 = 8'h0b,
             data_10     = 8'h02,

             reg_addr_11 = 8'h0c,
             data_11     = 8'h00,

             // green offset
             reg_addr_12 = 8'h0d,
             data_12     = 8'h02,

             reg_addr_13 = 8'h0e,
             data_13     = 8'h00,

             // blue offset
             reg_addr_14 = 8'h0f,
             data_14     = 8'h02,

             reg_addr_15 = 8'h10,
             data_15     = 8'h00,

             reg_addr_16 = 8'h18,
             data_16     = 8'h00,

             // hsync control
             reg_addr_17 = 8'h12,
             data_17     = 8'h80,

             // pll_div_msb {[7:0]=pll_div[11:4] }    // 800
             reg_addr_18 = 8'h01,
             data_18     = 8'h32,

             // pll_div_lsb {[7:4]=pll_div[3:0] }     // 800
             reg_addr_19 = 8'h02,
             data_19     = 8'h00,

             // clk_gen_ctrl {[7:6]=vco_range, [5:3]=charge_pump_current, [2]=ext_clk_en}
             reg_addr_20 = 8'h03,
             data_20     = 8'h48,

             // phase_adjust {[7:3]=phase_adjust}
             reg_addr_21 = 8'h04,
             data_21     = 8'ha0,

             // hsync control {...,[3]= hsync_out polarity}
             reg_addr_22 = 8'h12,
             data_22     = 8'h10,

             // hsync_duration {[7:0]= hsync_pulsewidth_duration }
             reg_addr_23 = 8'h13,
             data_23     = 8'h60,

             // vsync control {...,[3]= vsync_out polarity}
             reg_addr_24 = 8'h14,
             data_24     = 8'h10,

             // clamp_placement {[7:0]= clamp_placement }
             reg_addr_25 = 8'h19,
             data_25     = 8'h04,

             // clamp_duration {[7:0]= clamp_duration}
             reg_addr_26 = 8'h1a,
             data_26     = 8'h1a,

             /*
             // pll_div_msb {[7:0]=pll_div[11:4] }    // 800
             reg_addr_18 = 8'h01, 
             data_18     = 8'h42, 

             // pll_div_lsb {[7:4]=pll_div[3:0] }     // 800
             reg_addr_19 = 8'h02, 
             data_19     = 8'h00,

             // clk_gen_ctrl {[7:6]=vco_range, [5:3]=charge_pump_current, [2]=ext_clk_en}
             reg_addr_20 = 8'h03,
             data_20     = 8'h48,

             // phase_adjust {[7:3]=phase_adjust}
             reg_addr_21 = 8'h04,
             data_21     = 8'h80,

             // hsync control {...,[3]= hsync_out polarity} 
             reg_addr_22 = 8'h12,
             data_22     = 8'h10,

             // hsync_duration {[7:0]= hsync_pulsewidth_duration }
             reg_addr_23 = 8'h13,
             data_23     = 8'h60,

             // vsync control {...,[3]= vsync_out polarity}
             reg_addr_24 = 8'h14,
             data_24     = 8'h10,

             // clamp_placement {[7:0]= clamp_placement }
             reg_addr_25 = 8'h19,
             data_25     = 8'h04,

             // clamp_duration {[7:0]= clamp_duration}
             reg_addr_26 = 8'h1a,
             data_26     = 8'h3c,
             */
				  
             stop_bit       = 1'b0,
             sda_buffer_msb = 27;

  reg                          sda_out;
  reg                          scl_out;
  reg [transition_cycle_msb:0] cycle_count;
  reg [2:0]                    c_state;
  reg [2:0]                    n_state;
  reg [4:0]                    write_count;
  reg [31:0]                   bit_count;
  reg [sda_buffer_msb:0]       sda_buffer;

  wire transition;

  always @(posedge clock) begin
    if (reset || c_state == idle) begin
      sda_out <= 1'b1;
      scl_out <= 1'b1;
    end
    else if (c_state == init && transition) begin 
      sda_out <= 1'b0;
    end
    else if (c_state == setup) begin
      sda_out <= sda_buffer[sda_buffer_msb];
    end
    else if (c_state == clk_rise && cycle_count == transition_cycle/2 && bit_count == sda_buffer_msb) begin
      sda_out <= 1'b1;
    end
    else if (c_state == clk_fall) begin
      scl_out <= 1'b0;
    end
    else if (c_state == clk_rise) begin
      scl_out <= 1'b1;
    end
  end

  assign sda = sda_out;
  assign scl = scl_out;

  always @(posedge clock) begin
    //reset or end condition
    if (reset) begin
      sda_buffer  <= {slave_addr,write,ack,reg_addr0,ack,data0,ack,stop_bit};
      cycle_count <= 0;
    end
    //setup sda for sck rise
    else if (c_state == setup && cycle_count == transition_cycle) begin
      sda_buffer   <= {sda_buffer[sda_buffer_msb-1:0],1'b0};
      cycle_count  <= 0; 
    end
    //reset count at end of state
    else if (cycle_count == transition_cycle)
      cycle_count <= 0;
    else if (c_state == wait_state) begin
      case (write_count)
        0  : sda_buffer <= {slave_addr,write,ack,reg_addr1,ack,data1,ack,stop_bit};
        1  : sda_buffer <= {slave_addr,write,ack,reg_addr2,ack,data2b,ack,stop_bit};
        2  : sda_buffer <= {slave_addr,write,ack,reg_addr3,ack,data3b,ack,stop_bit};
        3  : sda_buffer <= {slave_addr,write,ack,reg_addr4,ack,data4b,ack,stop_bit};
        4  : sda_buffer <= {slave_addr_0,write,ack,reg_addr_0,ack,data_0,ack,stop_bit};
        5  : sda_buffer <= {slave_addr_0,write,ack,reg_addr_1,ack,data_1,ack,stop_bit};
        6  : sda_buffer <= {slave_addr_0,write,ack,reg_addr_2,ack,data_2,ack,stop_bit};
        7  : sda_buffer <= {slave_addr_0,write,ack,reg_addr_3,ack,data_3,ack,stop_bit};
        8  : sda_buffer <= {slave_addr_0,write,ack,reg_addr_4,ack,data_4,ack,stop_bit};
        9  : sda_buffer <= {slave_addr_0,write,ack,reg_addr_5,ack,data_5,ack,stop_bit};
        10 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_6,ack,data_6,ack,stop_bit};
        11 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_7,ack,data_7,ack,stop_bit};
        12 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_8,ack,data_8,ack,stop_bit};
        13 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_9,ack,data_9,ack,stop_bit};
        14 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_10,ack,data_10,ack,stop_bit};
        15 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_11,ack,data_11,ack,stop_bit};
        16 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_12,ack,data_12,ack,stop_bit};
        17 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_13,ack,data_13,ack,stop_bit};
        18 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_14,ack,data_14,ack,stop_bit};
        19 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_15,ack,data_15,ack,stop_bit};
        20 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_16,ack,data_16,ack,stop_bit};
        21 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_17,ack,data_17,ack,stop_bit};
        22 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_18,ack,data_18,ack,stop_bit};
        23 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_19,ack,data_19,ack,stop_bit};
        24 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_20,ack,data_20,ack,stop_bit};
        25 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_21,ack,data_21,ack,stop_bit};
        26 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_22,ack,data_22,ack,stop_bit};
        27 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_23,ack,data_23,ack,stop_bit};
        28 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_24,ack,data_24,ack,stop_bit};
        29 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_25,ack,data_25,ack,stop_bit};
        30 : sda_buffer <= {slave_addr_0,write,ack,reg_addr_26,ack,data_26,ack,stop_bit};
        default: sda_buffer <= 28'dx;
      endcase 
      cycle_count <= cycle_count + 1;
    end
    else
      cycle_count <= cycle_count + 1;
  end

  always @(posedge clock) begin 
    if (reset)
      write_count <= 5'd0;
    else if (c_state == wait_state && cycle_count == transition_cycle)
      write_count <= write_count + 1;
  end

  always @ (posedge clock) begin
    if (reset || (c_state == wait_state)) 
      bit_count <= 0;
    else if (c_state == clk_rise && cycle_count == transition_cycle)
      bit_count <= bit_count + 1;
  end

  always @ (posedge clock) begin
    if (reset)
      c_state <= init;
    else 
      c_state <= n_state;
  end

  assign transition = (cycle_count == transition_cycle);

  //next state
  always @ (*) begin
    case (c_state) 
      idle: begin
        if (reset) n_state = init;
        else n_state = idle;
      end

      init: begin
        if (transition) n_state = start;
        else n_state = init;
      end

      start: begin
        if (reset) n_state = init;
        else if (transition) n_state = clk_fall;
        else n_state = start;
      end

      clk_fall: begin
        if (reset) n_state = init;
        else if (transition) n_state = setup;
        else n_state = clk_fall;
      end

      setup: begin
        if (reset) n_state = init;
        else if (transition) n_state = clk_rise;
        else n_state = setup;
      end

      clk_rise: begin
        if (reset) 
          n_state = init;
        else if (transition && bit_count == sda_buffer_msb) 
          n_state = wait_state;
        else if (transition)
          n_state = clk_fall;  
        else n_state = clk_rise;
      end

      wait_state: begin
        if (reset | (transition && write_count != 5'd28)) 
          n_state = init;
        else if (transition)
          n_state = idle;  
        else n_state = wait_state;
      end

      default: n_state = idle;     
    endcase
  end
endmodule
