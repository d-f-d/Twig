<?php
/**
 * Created by PhpStorm.
 * User: punk_undead
 * Date: 04.01.16
 * Time: 23:33
 */

namespace d\Twig;


class Compiler extends \Twig_Compiler {
  function setSource($source) {
    $this->source = $source;
  }
}