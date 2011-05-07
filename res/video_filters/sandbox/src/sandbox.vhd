----------------------------------------------------------------------------------
-- Company: CCMLAB, Virginia Tech
-- Engineer: Tony Frangieh
-- 
-- Create Date:    15:18:30 04/09/2011 
-- Design Name: 
-- Module Name:    top - Behavioral 
-- Project Name: simple_filters
-- Target Devices: 
-- Tool versions: 
-- Description: Static design sandbox
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
library UNISIM;
use UNISIM.VComponents.all;

entity top is port(
  clock           : in    std_logic;                       -- System clock
  reset           : in    std_logic;                       -- Reset
  iic_sda         : inout std_logic;                       -- IIC sda
  iic_scl         : inout std_logic;                       -- IIC scl
  vga_red_in      : in    std_logic_vector (7 downto 0);   -- VGA red bus
  vga_green_in    : in    std_logic_vector (7 downto 0);   -- VGA green bus
  vga_blue_in     : in    std_logic_vector (7 downto 0);   -- VGA blue bus
  vga_in_data_clk : in    std_logic;                       -- VGA pixel clock
  vga_in_hsout    : in    std_logic;                       -- VGA horizontal sync
  vga_in_vsout    : in    std_logic;                       -- VGA vertical sync
  vga_in_clamp    : out   std_logic;                       -- VGA clamp
  vga_in_coast    : out   std_logic;                       -- VGA coast
  dvi_reset       : out   std_logic;                       -- DVI reset
  dvi_h           : out   std_logic;                       -- DVI horizontal sync
  dvi_v           : out   std_logic;                       -- DVI vertical sync
  dvi_de          : out   std_logic;                       -- DVI de
  dvi_xclk_n      : out   std_logic;                       -- DVI differential clock
  dvi_xclk_p      : out   std_logic;                       -- DVI differential clock
  dvi_d           : out   std_logic_vector (11 downto 0)); -- DVI out
end top;

architecture Behavioral of top is

  -- Input bus-macro components
  --attribute box_type: string;
  component busmacro_xc5v_async_vert_input_0 port(
    input0  : in std_logic;
    input1  : in std_logic;
    input2  : in std_logic;
    input3  : in std_logic;
    input4  : in std_logic;
    input5  : in std_logic;
    input6  : in std_logic;
    input7  : in std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  --attribute box_type of busmacro_xc5v_async_vert_input_0: component is "black_box";

  component busmacro_xc5v_async_vert_input_1 port(
    input0  : in std_logic;
    input1  : in std_logic;
    input2  : in std_logic;
    input3  : in std_logic;
    input4  : in std_logic;
    input5  : in std_logic;
    input6  : in std_logic;
    input7  : in std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  --attribute box_type of busmacro_xc5v_async_vert_input_1: component is "black_box";

  component busmacro_xc5v_async_vert_input_2 port(
    input0  : in std_logic;
    input1  : in std_logic;
    input2  : in std_logic;
    input3  : in std_logic;
    input4  : in std_logic;
    input5  : in std_logic;
    input6  : in std_logic;
    input7  : in std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  --attribute box_type of busmacro_xc5v_async_vert_input_2: component is "black_box";

  -- Output bus-macro components
  component busmacro_xc5v_async_vert_output_0 port(
    input0  : in std_logic;
    input1  : in std_logic;
    input2  : in std_logic;
    input3  : in std_logic;
    input4  : in std_logic;
    input5  : in std_logic;
    input6  : in std_logic;
    input7  : in std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  --attribute box_type of busmacro_xc5v_async_vert_output_0: component is "black_box";

  component busmacro_xc5v_async_vert_output_1 port(
    input0  : in std_logic;
    input1  : in std_logic;
    input2  : in std_logic;
    input3  : in std_logic;
    input4  : in std_logic;
    input5  : in std_logic;
    input6  : in std_logic;
    input7  : in std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  --attribute box_type of busmacro_xc5v_async_vert_output_1: component is "black_box";

  component busmacro_xc5v_async_vert_output_2 port(
    input0  : in std_logic;
    input1  : in std_logic;
    input2  : in std_logic;
    input3  : in std_logic;
    input4  : in std_logic;
    input5  : in std_logic;
    input6  : in std_logic;
    input7  : in std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  --attribute box_type of busmacro_xc5v_async_vert_output_2: component is "black_box";

  -- RGB to DVI converttion module
  component rgb2dvi port(
    clock      : in  std_logic;
    reset      : in  std_logic;
    hsync_in   : in  std_logic;
    vsync_in   : in  std_logic;
    de_in      : in  std_logic;
    rgb_in     : in  std_logic_vector (23 downto 0);
    dvi_h      : out std_logic;
    dvi_v      : out std_logic;
    dvi_de     : out std_logic;
    dvi_xclk_n : out std_logic;
    dvi_xclk_p : out std_logic;
    dvi_d      : out std_logic_vector (11 downto 0));
  end component;

  -- Video chips IIC configuration modules
  component iic_controller port(
    clock : in    std_logic;
    reset : in    std_logic;
    sda   : inout std_logic;
    scl   : inout std_logic);
  end component;

  -- Architecture signals
  signal de_in             : std_logic;
  signal component_iid_sda : std_logic;
  signal component_iid_scl : std_logic;
  signal rgb_in            : std_logic_vector (23 downto 0);

  signal passthrough : std_logic_vector(23 downto 0);
  signal data_reg_i  : std_logic_vector(23 downto 0);
  signal data_reg_o  : std_logic_vector(23 downto 0);

  begin
    vga_in_clamp <= '0';
    vga_in_coast <= '0';
    dvi_reset    <= reset;

    component_init: iic_controller port map(clock, not(reset), component_iid_sda, component_iid_scl);
    sda: obuft_lvcmos18 port map(o => iic_sda, i => component_iid_sda, t => '0');
    scl: obuft_lvcmos18 port map(o => iic_scl, i => component_iid_scl, t => '0');

    rgb_in <= vga_blue_in & vga_green_in & vga_red_in;

    process(clock,reset) begin
      if (not(reset) = '1') then
        data_reg_i <= (others => '0');
      elsif(clock='1' and clock'event) then
        data_reg_i(23 downto 0)  <= rgb_in;
      end if;
    end process;

    -- In the static sandbox, connect input of dynamic region to its output
    -- Input bus-macros
    busmacro_xc5v_async_vert_input_0_0 : busmacro_xc5v_async_vert_input_0 port map(data_reg_i(0),data_reg_i(1),data_reg_i(2),data_reg_i(3),
                                                                                   data_reg_i(4),data_reg_i(5),data_reg_i(6),data_reg_i(7),
                                                                                   passthrough(0),passthrough(1),passthrough(2),passthrough(3),
                                                                                   passthrough(4),passthrough(5),passthrough(6),passthrough(7));

    busmacro_xc5v_async_vert_input_1_1 : busmacro_xc5v_async_vert_input_1 port map(data_reg_i(8),data_reg_i(9),data_reg_i(10),data_reg_i(11),
                                                                                   data_reg_i(12),data_reg_i(13),data_reg_i(14),data_reg_i(15),
                                                                                   passthrough(8),passthrough(9),passthrough(10),passthrough(11),
                                                                                   passthrough(12),passthrough(13),passthrough(14),passthrough(15));

    busmacro_xc5v_async_vert_input_2_2 : busmacro_xc5v_async_vert_input_2 port map(data_reg_i(16),data_reg_i(17),data_reg_i(18),data_reg_i(19),
                                                                                   data_reg_i(20),data_reg_i(21),data_reg_i(22),data_reg_i(23),
                                                                                   passthrough(16),passthrough(17),passthrough(18),passthrough(19),
                                                                                   passthrough(20),passthrough(21),passthrough(22),passthrough(23));

    -- Output bus-macros
    busmacro_xc5v_async_vert_output_0_0 : busmacro_xc5v_async_vert_output_0 port map(passthrough(0), passthrough(1), passthrough(2), passthrough(3),
                                                                                     passthrough(4), passthrough(5), passthrough(6), passthrough(7),
                                                                                     data_reg_o(0), data_reg_o(1), data_reg_o(2), data_reg_o(3),
                                                                                     data_reg_o(4), data_reg_o(5), data_reg_o(6), data_reg_o(7));

    busmacro_xc5v_async_vert_output_1_1 : busmacro_xc5v_async_vert_output_1 port map(passthrough(8), passthrough(9), passthrough(10), passthrough(11),
                                                                                     passthrough(12), passthrough(13), passthrough(14), passthrough(15),
                                                                                     data_reg_o(8), data_reg_o(9), data_reg_o(10), data_reg_o(11),
                                                                                     data_reg_o(12), data_reg_o(13), data_reg_o(14), data_reg_o(15));

    busmacro_xc5v_async_vert_output_2_2 : busmacro_xc5v_async_vert_output_2 port map(passthrough(16), passthrough(17), passthrough(18), passthrough(19),
                                                                                     passthrough(20), passthrough(21), passthrough(22), passthrough(23),
                                                                                     data_reg_o(16), data_reg_o(17), data_reg_o(18), data_reg_o(19),
                                                                                     data_reg_o(20), data_reg_o(21), data_reg_o(22), data_reg_o(23));

    de_in <= vga_in_hsout and not(vga_in_vsout);

    -- RGV to DVI module instantiation
    dvi_output: rgb2dvi port map(vga_in_data_clk, not(reset), vga_in_hsout, not(vga_in_vsout), de_in, data_reg_o(23 downto 0), dvi_h, dvi_v, dvi_de, dvi_xclk_n, dvi_xclk_p, dvi_d);

end Behavioral;
-- end of file
