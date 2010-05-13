<?php

class MailhandlerCommandsDefault {
   protected $commands = NULL;
  
  /**
   * Parse the commands
   */
   function parse($item, &$node) {
     watchdog('commands', 'parsing commands');
     $body = $item['body'];
     // Prepend the default commands.  User-added commands will override the default commands.
     if ($item['commands']) {
       $body = trim($item['commands']) ."\n". $body;
     }
     $commands = array(); $endcommands = NULL;
     // Collect the commands and locate signature
     $lines = explode("\n", $body);
     for ($i = 0; $i < count($lines); $i++) {
       $line = trim($lines[$i]);
       $words = explode(' ', $line);
       // Look for a command line. if not present, note which line number is the boundary
       if (substr($words[0], -1) == ':' && !isset($endcommands)) {
         // Looks like a name: value pair
         $commands[$i] = explode(': ', $line, 2);
       }
       else {
         if (!isset($endcommands)) $endcommands = $i;
       }
     }
     $this->commands = $commands;
   }

  /**
   * Parse and process commands
   */
  function process($item, &$node) {
    watchdog('commands', 'processing commands');
    dwd('parsed commands', $this->commands);
  }
  

}