----------------------------------------------------------------------------------
-- Company: CCMLAB, Virginia Tech
-- Engineer: Tony Frangieh
-- 
-- Create Date:    16:00:38 04/09/2011 
-- Design Name: 
-- Module Name:    prm - Behavioral 
-- Project Name: simple_filters
-- Target Devices: 
-- Tool versions: 
-- Description: Passthrough filter
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

entity prm is port(
  clock   : in  std_logic;                       -- RGB pixel clock
  rgb_in : in  std_logic_vector (23 downto 0);   -- RGB input 
  rgb_out : out std_logic_vector (23 downto 0)); -- RGB output (passthrough)
end prm;

architecture behavioral of prm is begin
  process (clock) begin
  if (clock'event and clock = '1') then
    -- Passthrough
    rgb_out <= rgb_in;
  end if;
  end process;
end behavioral;
