----------------------------------------------------------------------------------
-- Company: CCMLAB, Virginia Tech
-- Engineer: Tony Frangieh
-- 
-- Create Date:    15:50:01 04/09/2011 
-- Design Name: 
-- Module Name:    prm - Behavioral 
-- Project Name: simple_filters
-- Target Devices: 
-- Tool versions: 
-- Description: Intensity filter
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
  clock   : in  std_logic;                       -- Clock
  rgb_in  : in  std_logic_vector (23 downto 0);  -- RGB input
  rgb_out : out std_logic_vector (23 downto 0)); -- RGB output
end prm;

architecture prm_behav of prm is
  --intensity level
  shared variable intensity_level : integer range -255 to 255 := 128;

  begin					
  process(clock)
    --process variables
    variable temp_red : integer range -255 to 510 := 0;
    variable temp_green : integer range -255 to 510 := 0;
    variable temp_blue : integer range -255 to 510 := 0;
    begin
    if clock'event and clock = '1' then
      --calculate the new rgb values by adding intensity_level to each color
      temp_red   := intensity_level + conv_integer(unsigned(rgb_in(7 downto 0)));
      temp_green := intensity_level + conv_integer(unsigned(rgb_in(15 downto 8)));
      temp_blue  := intensity_level + conv_integer(unsigned(rgb_in(23 downto 16)));

      --if computed red value greater than 255, set output red value to 255
      if(temp_red > 255) then
        rgb_out(7 downto 0) <= (others => '1');
        --if computed red value less than 0, set output red value to 0
      elsif(temp_red < 0) then
        rgb_out(7 downto 0) <= (others => '0');
        --else, set the output red value to the computed value
      else
        rgb_out(7 downto 0) <= conv_std_logic_vector(temp_red,8);
      end if;

      --if computed green value greater than 255, set output green value to 255
      if(temp_green > 255) then
        rgb_out(15 downto 8) <= (others => '1');
        --if computed green value less than 0, set output green value to 0
      elsif(temp_green < 0) then
        rgb_out(15 downto 8) <= (others => '0');
      else
        --else, set the output green value to the computed value
        rgb_out(15 downto 8) <= conv_std_logic_vector(temp_green,8);
      end if;

      --if computed blue value greater than 255, set output blue value to 255
      if(temp_blue > 255) then
        rgb_out(23 downto 16) <= (others => '1');
        --if computed blue value less than 0, set output blue value to 0
      elsif(temp_blue < 0) then
        rgb_out(23 downto 16) <= (others => '0');
        --else, set the output blue value to the computed value
      else
        rgb_out(23 downto 16) <= conv_std_logic_vector(temp_blue,8);
      end if;
    end if;
  end process;
end prm_behav;
