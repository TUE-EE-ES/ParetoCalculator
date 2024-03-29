package org.xtext.paretoalgebra.calculator.generator

import com.google.inject.Inject
import org.eclipse.emf.ecore.resource.Resource
import org.eclipse.xtext.generator.AbstractGenerator
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext
import org.eclipse.xtext.naming.IQualifiedNameProvider
import org.xtext.paretoalgebra.calculator.calculator.EnumValues
import org.xtext.paretoalgebra.calculator.calculator.ParetoModel
import org.xtext.paretoalgebra.calculator.calculator.QuantityType
import org.xtext.paretoalgebra.calculator.calculator.ConfSpace
import org.xtext.paretoalgebra.calculator.calculator.ConfSet
import org.xtext.paretoalgebra.calculator.calculator.COperation

/**
 * Generates code from your model files on save.
 * 
 * See https://www.eclipse.org/Xtext/documentation/303_runtime_concepts.html#code-generation
 */
class CalculatorGenerator extends AbstractGenerator {

	@Inject extension IQualifiedNameProvider



	override void doGenerate(Resource resource, IFileSystemAccess2 fsa, IGeneratorContext context) {


        for (m : resource.allContents.toIterable.filter(ParetoModel)) {
	        fsa.generateFile(
	            m.fullyQualifiedName.toString("/") + ".xml",
	            m.compile)                
        }
	}
	

	def compile(EnumValues en) '''
	«FOR v: en.values»

		<value>«v»</value>

	«ENDFOR»

	'''

	def gettype(QuantityType qt) '''
		«qt.qttype.QTREAL»«qt.qttype.QTINT»


		«IF qt.qttype.QTORDERED !== null»ordered«ENDIF»


		«IF qt.qttype.QTUNORDERED !== null»unordered«ENDIF»


	'''

	def getbody(QuantityType qt) '''
		«IF qt.qttype.QTORDERED !== null»

			<values>
				«qt.qttype.QTORDERED.values.compile»

			</values>
		«ENDIF»

		«IF qt.qttype.QTUNORDERED !== null»

			<values>
				«qt.qttype.QTUNORDERED.values.compile»

			</values>
		«ENDIF»

		'''


	def compile(QuantityType qt) '''
		<quantity_definition name="«qt.qtname»" type="«qt.gettype.toString().trim()»">


		«qt.getbody»

	    </quantity_definition>
	'''

	def getbody(ConfSpace csp) '''
		«FOR qt: csp.quantities»

		<quantity name="«qt.quant»"«IF qt.name !== null» referBy="«qt.name»"«ENDIF»>




		</quantity>
		«ENDFOR»

		'''

	def compile(ConfSpace csp) '''
		<configuration_space name="«csp.name»">

		«csp.getbody»

	    </configuration_space>
	'''

	def compile(ConfSet cs) '''
		<configuration_set name="«cs.name»" space_id="«cs.space»">


		<configurations>
		«FOR c: cs.set.confs»

			<configuration>
			«FOR v: c.values»

				<value>«v.numeric»«v.id»</value>


			«ENDFOR»

			</configuration>
		«ENDFOR»

		</configurations>
	    </configuration_set>
	'''


	
	def compileQuantityTypes(ParetoModel m) '''
		<quantity_definitions>
	        «FOR qt:m.qtypes»

	            «qt.compile»

	        «ENDFOR»

        </quantity_definitions>
     '''

	def compileConfigurationSets(ParetoModel m) '''
		<configuration_sets>
	    «FOR cs:m.confsets»

	    	«cs.compile»

	    «ENDFOR»

		</configuration_sets>
     '''


	def compileConfigurationSpaces(ParetoModel m) '''
		<configuration_spaces>
	        «FOR csp:m.confspaces»

	            «csp.compile»

	        «ENDFOR»

        </configuration_spaces>
     '''

	def compile(COperation o) '''
		«IF o.op_push !== null»<push name="«o.op_push.quantity»"/>«ENDIF»



		«IF o.op_prod !== null»<product/>«ENDIF»


		«IF o.op_prodcons !== null»

		<prodcons>
			<producer_quant>«o.op_prodcons.prodquant»</producer_quant>

			<consumer_quant>«o.op_prodcons.consquant»</consumer_quant>

		</prodcons>
		«ENDIF»

		«IF o.op_abstract !== null»

		<abstract>
			«FOR q:o.op_abstract.list.qnames»

				<quant>«q»</quant>

			«ENDFOR»

		</abstract>
		«ENDIF»

		«IF o.op_hide !== null»

		<hide>
			«FOR q:o.op_hide.list.qnames»

				<quant>«q»</quant>

			«ENDFOR»

		</hide>
		«ENDIF»

		«IF o.op_minimize !== null»

		<minimize/>
		«ENDIF»

		«IF o.op_store!== null»

		<store name="«o.op_store.name»"/>

		«ENDIF»

		«IF o.op_join!== null»

		<join>
		<between quanta="«o.op_join.qa»" quantb="«o.op_join.qb»"/>


		</join>
		«ENDIF»

		«IF o.op_aggregate !== null»

		<aggregate>
			«FOR q:o.op_aggregate.list.qnames»

			<quant>«q»</quant>

			«ENDFOR»

			<name>«o.op_aggregate.aggrname»</name>

		</aggregate>
		«ENDIF»

		«IF o.op_duplicate !== null»

		<duplicate/>
		«ENDIF»

		«IF o.op_print !== null»

		<print/>
		«ENDIF»

	'''

	def compileCalculation(ParetoModel m) '''
		<calculation>
        «FOR c:m.calc.operations»

            «c.compile»

        «ENDFOR»

		</calculation>
     '''




	def compile(ParetoModel m) '''
		<?xml version="1.0"?>
		<?xml-stylesheet type="text/xsl" href="../xml/paretospec.xslt"?>
		<pareto_specification xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns="uri:pareto" xsi:schemaLocation="uri:pareto http://www.es.ele.tue.nl/~mgeilen/paretoalgebra/xml/paretospec.xsd ">
			<name>«m.name»</name>

  			<description>
		    	«m.descr.block.substring(1,m.descr.block.length()-2).trim()» 

			</description>
    		«this.compileQuantityTypes(m)»

    		«this.compileConfigurationSpaces(m)»

    		«this.compileConfigurationSets(m)»

    		«this.compileCalculation(m)»

		</pareto_specification>
    '''


}

