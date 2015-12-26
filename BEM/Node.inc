<?php
namespace d\Twig\BEM;

use Twig_Compiler;
use Twig_Node;

class Node extends Twig_Node {
  /**
   * @var Twig_Compiler
   */
  private $compiler;

  /**
   * @param \Twig_Compiler $compiler
   * @return $this
   */
  private function start(Twig_Compiler $compiler) {
    $this->compiler = $compiler;
    $this->compiler
      ->addDebugInfo($this);
    return $this;
  }

  /**
   * @return $this
   */
  private function echoBlock() {
    $this->compiler
      ->raw(' . $context[\'block\']');
    return $this;
  }

  /**
   * @return $this
   */
  private function echoMore() {
    $more = $this->getNode('more');
    $c = get_class($more);
    if (($c === 'Twig_Node_Expression_Constant')) {
      $this->echoSpace();
      $this->compiler
        ->raw(' . ')
        ->subcompile($more);
    }
    else {
      $v = '$v_' . mt_rand(0, 9999);
      $this->compiler
        ->raw(' . ')
        ->raw('((' . $v . ' = ')
        ->subcompile($more)
        ->raw(') ? (" " . ' . $v . ') : "")');
    }
    return $this;
  }

  /**
   * @return $this
   */
  private function echoElement() {
    $this->compiler
      ->raw(' . \'__\' . ')
      ->subcompile($this->getNode('element'));
    return $this;
  }

  /**
   * @return $this
   */
  private function echoSpace() {
    $this->compiler
      ->raw(' . \' \'');
    return $this;
  }

  /**
   * @return $this
   */
  private function echoMod() {
    $mod = $this->getNode('mod');
    $c = get_class($mod);
    if ($c === 'Twig_Node_Expression_Constant' && empty($mod->attributes['value'])) {
      return $this;
    }
    if ($c === '!Twig_Node_Expression_Array') {
      for ($i = 1; $i < count($mod->nodes); $i += 2) {
        $this->element();
        $this->compiler->raw(' . \'--\' . ');
        $this->compiler
          ->subcompile($mod->nodes[$i]);
      }
    }
    elseif ($c === 'Twig_Node_Expression_Constant') {
      $this->echoSpace();
      $this->element();
      $this->compiler->raw(' . \'--\' . ');
      $this->compiler->subcompile($mod);
    }
    else {
      $this->compiler->raw(' . (($mod_a = array_filter(is_array($mod = ');
      $this->compiler->subcompile($mod);
      $this->compiler->raw(') ? $mod : [$mod])) ? (($be = (" "');
      $this->element();
      $this->compiler->raw(' . \'--\'');
      $this->compiler->raw(')) . implode($be, $mod_a)) : "")');
    }
    return $this;
  }

  public function __construct(array $nodes = [], array $attributes = [], $line, $tag = NULL) {
    parent::__construct($nodes, $attributes, $line, $tag);
  }

  public function compile(Twig_Compiler $compiler) {
    $this->start($compiler);
    if ($this->getAttribute('method') !== 'setBlock') {
      $this->compiler->write('echo "class=\'"');
      $this->{$this->getAttribute('method')}($compiler);
      $this->compiler->raw(' . "\'"')->raw(";\n");
    }
    else {
      $this->{$this->getAttribute('method')}($compiler);
    }
  }

  private function setBlock(Twig_Compiler $compiler) {
    $compiler
      ->write('$context[\'block\'] = ')
      ->subcompile($this->getNode('block'))
      ->raw(";\n");
  }

  /**
   * @return $this
   */
  private function block() {
    $this->echoBlock();
    return $this;
  }

  /**
   * @return $this
   */
  private function blockMore() {
    $this
      ->block()
      ->echoMore();
    return $this;
  }

  /**
   * @return $this
   */
  private function element() {
    $this->echoBlock()->echoElement();
    return $this;
  }

  /**
   * @return $this
   */
  private function elementMod() {
    $this
      ->element()
//      ->echoSpace()
      ->echoMod();
    return $this;
  }

  /**
   * @return $this
   */
  private function elementModMore() {
    $this
      ->elementMod()
      ->echoMore();
    return $this;
  }
}