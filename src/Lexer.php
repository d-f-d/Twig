<?php
/**
 * Created by PhpStorm.
 * User: punk_undead
 * Date: 12.02.16
 * Time: 1:41
 */

namespace dfd\Twig;

use Twig_Lexer;
use Twig_LexerInterface;

class Lexer extends Twig_Lexer implements Twig_LexerInterface {

  function tokenize($code, $filename = NULL) {
    $code = str_replace(array("\r\n", "\r"), "\n", $code);
    ob_start();
    eval('namespace tmpnamespace_' .md5($code)  . ';'.'?>' . $code);
    $code = ob_get_clean();
    return parent::tokenize($code, $filename);
  }
}