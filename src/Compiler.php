<?php
/**
 * Created by PhpStorm.
 * User: punk_undead
 * Date: 04.01.16
 * Time: 23:33
 */

namespace dfd\Twig;

use Twig_Compiler;

class Compiler extends Twig_Compiler {

  /**
   * @param string $source
   */
  function setSource($source) {
    $this->source = $source;
  }
}