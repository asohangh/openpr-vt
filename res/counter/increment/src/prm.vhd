----------------------------------------------------------------------------------
-- Company: CCMLAB, Virginia Tech
-- Engineers: Ali Sohanghpurwala & Tony Frangieh
-- 
-- Create Date:    22:33:58 04/25/2011 
-- Design Name: 
-- Module Name:    prm - Behavioral 
-- Project Name: counter
-- Target Devices: 
-- Tool versions: 
-- Description: Increment input by 1
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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity prm is port(
  clk      : in  std_logic;
  data_in  : in  std_logic_vector(7 downto 0);
  data_out : out std_logic_vector(7 downto 0));
end prm;

architecture prm of prm is begin
  process(clk) begin
    if(clk = '1' and clk'event) then
      data_out <= data_in + 1;
    end if;
  end process;

end architecture prm;

