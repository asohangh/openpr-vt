----------------------------------------------------------------------------------
-- Company: CCMLAB, Virginia Tech
-- Engineer: Tony Frangieh
-- 
-- Create Date:    15:59:16 04/09/2011 
-- Design Name: 
-- Module Name:    prm - Behavioral 
-- Project Name: simple_filters
-- Target Devices: 
-- Tool versions: 
-- Description: Negative filter
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
  clock   : in  std_logic;                       -- Clock
  rgb_in  : in  std_logic_vector (23 downto 0);  -- RGB in
  rgb_out : out std_logic_vector (23 downto 0)); -- RGB output
end prm;

architecture prm_behav of prm is begin
  process(clock)
  begin
  if (clock'event and clock = '1') then
    rgb_out <= not(rgb_in);
  end if;
  end process;
end prm_behav;
