-- uart_fsm.vhd: UART controller - finite state machine
-- Author: David Chocholaty <xchoch09@stud.fit.vutbr.cz> 
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK:       in std_logic;
   RST:       in std_logic;
   DIN: 	     in std_logic;
   BIT_MID:   in std_logic;  -- Middle of bit
   DATA_RDY:  in std_logic;  -- All eight bits are read
   DATA_READ: out std_logic; -- FSM OUTPUT reading data
   DATA_VLD:  out std_logic  -- FSM OUTPUT data are valid 
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
   type t_state is (S_idle, S_read, S_stop, S_valid);
   signal cur_state : t_state := S_idle;
   signal next_state : t_state := S_idle;   

begin
   -- State register   
   state_reg: process (CLK, RST)   
   begin
      if RST = '1' then
         cur_state <= S_idle;    
      elsif rising_edge(CLK) then
         -- State is changed on clock rising edge
         cur_state <= next_state;                 
      end if;
   end process;

   -- Next state logic      
   next_state_logic: process (BIT_MID)
   begin             
      next_state <= cur_state;      

      case cur_state is
         when S_idle =>
            -- Test valid start bit   
            if (BIT_MID = '1' and DIN = '0') then               
               next_state <= S_read;                                                                          
            end if;            
         when S_read => 
            -- Test if all eight bits are read            
            if (BIT_MID = '1' and DATA_RDY = '1') then               
               next_state <= S_stop;               
            end if;
         when S_stop =>      
            -- Test valid stop bit                   
            if (BIT_MID = '1' and DIN = '1') then                           
               next_state <= S_valid;               
            end if; 
         when S_valid =>                      
            next_state <= S_idle;             
         when others =>
            next_state <= S_idle;
      end case;      
   end process;

   -- Output logic
   output_logic: process (cur_state)
   begin            
      case cur_state is
         when S_idle =>
            DATA_READ <= '0'; 
            DATA_VLD <= '0';              
         when S_read =>
            DATA_READ <= '1';
             DATA_VLD <= '0';
         when S_stop =>
            DATA_READ <= '0'; 
            DATA_VLD <= '0';
         when S_valid =>
            DATA_READ <= '0'; 
            DATA_VLD <= '1';              
         when others =>
            DATA_READ <= '0';
            DATA_VLD <= '0';
      end case;      
   end process;
end behavioral;