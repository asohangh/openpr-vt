----------------------------------------------------------------------------------
-- Company: CCMLAB, Virginia Tech
-- Engineers: Ali Sohanghpurwala & Tony Frangieh
-- 
-- Create Date:    22:48:31 04/25/2011 
-- Design Name: 
-- Module Name:    top - Behavioral 
-- Project Name: counter
-- Target Devices: 
-- Tool versions: 
-- Description: Dynamic design sandbox
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
--library UNISIM;
--use UNISIM.VComponents.all;

entity top is port(
  reset : in  std_logic;                      -- Reset
  clk   : in  std_logic;                      -- Clock
  dip   : in  std_logic_vector(7 downto 0);   -- Dip switches
  led   : out std_logic_vector(7 downto 0));  -- LEDs
end top;

architecture top of top is
  signal data_reg_i        : std_logic_vector(7 downto 0);
  signal data_reg_o        : std_logic_vector(7 downto 0);
  signal module_in         : std_logic_vector(7 downto 0);
  signal module_out        : std_logic_vector(7 downto 0);

  attribute box_type       : string;

  -- Input bus-macro component
  component busmacro_xc5v_async_vert_input_0 port(
    input0  : in  std_logic;
    input1  : in  std_logic;
    input2  : in  std_logic;
    input3  : in  std_logic;
    input4  : in  std_logic;
    input5  : in  std_logic;
    input6  : in  std_logic;
    input7  : in  std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  attribute box_type of busmacro_xc5v_async_vert_input_0 : component is "black_box";

  -- Output bus-macro component
  component busmacro_xc5v_async_vert_output_0 port(
    input0  : in  std_logic;
    input1  : in  std_logic;
    input2  : in  std_logic;
    input3  : in  std_logic;
    input4  : in  std_logic;
    input5  : in  std_logic;
    input6  : in  std_logic;
    input7  : in  std_logic;
    output0 : out std_logic;
    output1 : out std_logic;
    output2 : out std_logic;
    output3 : out std_logic;
    output4 : out std_logic;
    output5 : out std_logic;
    output6 : out std_logic;
    output7 : out std_logic);
  end component;
  attribute box_type of busmacro_xc5v_async_vert_output_0 : component is "black_box";

  -- Reconfigurable module : decrement input by 1
  component prm port(
    clk      : in  std_logic;
    data_in  : in  std_logic_vector(7 downto 0);
    data_out : out std_logic_vector(7 downto 0));
  end component;

  begin
    process(clk,reset) begin
      if (reset = '1') then
        data_reg_i <= (others => '0');
        led        <= (others => '0');
      elsif(clk='1' and clk'event) then
        data_reg_i <= dip;
        led        <= data_reg_o;
      end if;
    end process;

    -- Input bus-macro
    busmacro_xc5v_async_vert_input_0_0 : busmacro_xc5v_async_vert_input_0 port map(
      data_reg_i(0), data_reg_i(1), data_reg_i(2), data_reg_i(3), data_reg_i(4), data_reg_i(5), data_reg_i(6), data_reg_i(7),
      module_in(0), module_in(1), module_in(2), module_in(3), module_in(4), module_in(5), module_in(6), module_in(7));

    -- Reconfigurable module instantiation
    decrement : prm port map(
      clk, module_in,
      module_out);

    -- Output bus-macro
    busmacro_xc5v_async_vert_output_0_0 : busmacro_xc5v_async_vert_output_0 port map(
      module_out(0), module_out(1), module_out(2), module_out(3), module_out(4), module_out(5), module_out(6), module_out(7),
      data_reg_o(0), data_reg_o(1), data_reg_o(2), data_reg_o(3), data_reg_o(4), data_reg_o(5), data_reg_o(6), data_reg_o(7));

end architecture top;
