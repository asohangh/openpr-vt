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
-- Description: Grayscale filter
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
  rgb_in  : in  std_logic_vector (23 downto 0);  -- RGB in
  rgb_out : out std_logic_vector (23 downto 0)); -- RGB output
end prm;

architecture prm_behav of prm is begin
  process(clock)
    --process variables
    variable grayscale_value : integer range 0 to 255 := 0;
    variable temp_red        : integer range -255 to 510 := 0;
    variable temp_green      : integer range -255 to 510 := 0;
    variable temp_blue       : integer range -255 to 510 := 0;

    begin
    if (clock'event and clock = '1') then
      -- calculate the new rgb values by adding intensity_level to each color
      temp_red   := conv_integer(unsigned(rgb_in(7 downto 0)));
      temp_green := conv_integer(unsigned(rgb_in(15 downto 8)));
      temp_blue  := conv_integer(unsigned(rgb_in(23 downto 16)));				

      if (temp_red > temp_green) then       -- red value greater than green value
        if (temp_red > temp_blue) then      -- red value greater than blue value
          grayscale_value := temp_red;
        else                                -- red value smaller than blue value
          grayscale_value := temp_blue;
        end if;
      else                                  -- red value smaller than green value
        if (temp_green > temp_blue) then    -- green value greater than blue value
          grayscale_value := temp_green;
        else                                -- green value smaller than blue value
          grayscale_value := temp_blue;
        end if;
      end if;

      -- set the output red value to the computed value
      rgb_out(7 downto 0) <= conv_std_logic_vector(grayscale_value,8);
      -- set the output green value to the computed value
      rgb_out(15 downto 8) <= conv_std_logic_vector(grayscale_value,8);
      -- set the output blue value to the computed value
      rgb_out(23 downto 16) <= conv_std_logic_vector(grayscale_value,8);
    end if;
  end process;
end prm_behav;
