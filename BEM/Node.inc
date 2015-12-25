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
    $v = '$v_' . mt_rand(0, 9999);
    $this->compiler
      ->raw(' . ')
      ->raw('((' . $v . ' = ')
      ->subcompile($this->getNode('more'))
      ->raw(') ? (" " . ' . $v . ') : "")');
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
    $this->compiler
      ->raw(' . \'--\' . ');

    $mod = $this->getNode('mod');
    $c = get_class($mod);
    if ($c === 'Twig_Node_Expression_Array') {
      $this->compiler
        ->raw('implode(\' \' . $context[\'block\'] . \'__\' . ')
        ->subcompile($this->getNode('element'))
        ->raw(' . \'--\', ');
      $this->compiler->raw('[');
      for ($i = 1; $i < count($mod->nodes); $i += 2) {
        if ($i > 1) {
          $this->compiler->raw(', ');
        }
        $this->compiler
          ->subcompile($mod->nodes[$i]);
      }
      $this->compiler->raw('])');
    }
    elseif ($c === 'Twig_Node_Expression_Constant') {
      $this->compiler->subcompile($mod);
    }
    else {
      $this->compiler->raw('(is_array($mod = ');
      if ($c === "Twig_Node_Expression_Name") {
        $this->compiler
          ->raw('$context[\'' . $mod->getAttribute('name') . '\']');
      }
      else {
        $this->compiler
          ->subcompile($this->getNode('mod'));
      };
      $this->compiler->raw(') ? ')
        ->raw('implode(\' \' . $context[\'block\'] . \'__\' . ')
        ->subcompile($this->getNode('element'))
        ->raw(' . \'--\', $mod) : $mod)');
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
      $this->compiler->raw('. "\'"')->raw(";\n");
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
      ->echoSpace()
      ->element()
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