----------------------------------------------------------------------------------
-- Company: CCMLAB, Virginia Tech
-- Engineer: Tony Frangieh
-- 
-- Create Date:    15:09:48 04/09/2011 
-- Design Name: 
-- Module Name:    rgb2dvi - Behavioral 
-- Project Name: simple_filters
-- Target Devices: 
-- Tool versions: 
-- Description: RGB to DVI fprmat converter
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

entity rgb2dvi is port(
  clock      : in  std_logic;                       -- RGB pixel clock
  reset      : in  std_logic;                       -- Reset
  hsync_in   : in  std_logic;                       -- RGB horizontal sync
  vsync_in   : in  std_logic;                       -- RGB vertical sync
  de_in      : in  std_logic;
  rgb_in     : in  std_logic_vector (23 downto 0);  -- RGB in
  dvi_h      : out std_logic;                       -- DVI horizontal sync
  dvi_v      : out std_logic;                       -- DVI vertical sync
  dvi_de     : out std_logic;
  dvi_xclk_n : out std_logic;                       -- DVI differential clock
  dvi_xclk_p : out std_logic;                       -- DVI differential clock
  dvi_d      : out std_logic_vector (11 downto 0)); -- DVI out
end rgb2dvi;

architecture Behavioral of rgb2dvi is begin
  -- DVI horizontal sync
  fdrse_hsync: fdrse port map(q => dvi_h,c => clock,ce => '1',r => reset,d => hsync_in);
  -- DVI vertical sync
  fdrse_vsync: fdrse port map(q => dvi_v,c => clock,ce => '1',r => reset,d => vsync_in);
  -- DVI de
  fdrse_de   : fdrse port map(q => dvi_de,c => clock,ce => '1',r => reset,d => de_in);
  -- DVI differential clock
  oddr_dvi_xclk_n: oddr port map(q => dvi_xclk_n,c => clock,ce => '1',r => reset,d1 => '1',d2 => '0',s => '0');
  oddr_dvi_xclk_p: oddr port map(q => dvi_xclk_p,c => clock,ce => '1',r => reset,d1 => '0',d2 => '1',s => '0');
  -- DVI out
  oddr_dvi_d0:  oddr port map(q => dvi_d(0), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(16),d2 => rgb_in(12),s => '0');
  oddr_dvi_d1:  oddr port map(q => dvi_d(1), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(17),d2 => rgb_in(13),s => '0');
  oddr_dvi_d2:  oddr port map(q => dvi_d(2), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(18),d2 => rgb_in(14),s => '0');
  oddr_dvi_d3:  oddr port map(q => dvi_d(3), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(19),d2 => rgb_in(15),s => '0');
  oddr_dvi_d4:  oddr port map(q => dvi_d(4), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(20),d2 => rgb_in(0), s => '0');
  oddr_dvi_d5:  oddr port map(q => dvi_d(5), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(21),d2 => rgb_in(1), s => '0');
  oddr_dvi_d6:  oddr port map(q => dvi_d(6), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(22),d2 => rgb_in(2), s => '0');
  oddr_dvi_d7:  oddr port map(q => dvi_d(7), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(23),d2 => rgb_in(3), s => '0');
  oddr_dvi_d8:  oddr port map(q => dvi_d(8), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(8), d2 => rgb_in(4), s => '0');
  oddr_dvi_d9:  oddr port map(q => dvi_d(9), c => clock,ce => '1',r => not(de_in),d1 => rgb_in(9), d2 => rgb_in(5), s => '0');
  oddr_dvi_d10: oddr port map(q => dvi_d(10),c => clock,ce => '1',r => not(de_in),d1 => rgb_in(10),d2 => rgb_in(6), s => '0');
  oddr_dvi_d11: oddr port map(q => dvi_d(11),c => clock,ce => '1',r => not(de_in),d1 => rgb_in(11),d2 => rgb_in(7), s => '0');
end Behavioral;
