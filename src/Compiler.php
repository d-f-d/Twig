<?php

namespace dfd\Twig;

use Twig_Compiler;

class Compiler extends Twig_Compiler {

  /**
   * @param string $source
   */
  function setSource(string $source) {
    $this->source = $source;
  }
}
