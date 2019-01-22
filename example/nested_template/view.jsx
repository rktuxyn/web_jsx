#extends /report/assets/header.main
<impl-placeholder id="main_body">
<js::
	let stmt = __pgsql.parse_param([],` 
		SELECT 
			*
		my_schema.my_data
		where ta.transaction_analytics_id = ${__def.transaction_analytics_id}
	`);
	let resp = __pgsql.execute_io("__sql_execute", {
		sql: stmt
	});
	if ( resp.ret_val < 0 || resp.ret_data_table === null || resp.ret_data_table === "" ){
		if(resp.ret_data_table === null || resp.ret_data_table === ""){
			context.response.write("No Data Found!!!");
		}else{
			context.response.write(resp.ret_msg);
		}
	}else {
		let row = JSON.parse(resp.ret_data_table)[0];
		delete resp.ret_data_table;
		let currency = require( "/addon/currency_in_word.js" );
::js>
	<div class="invoice-title">
		<h4>Transaction No # <js= __def.transaction_analytics_id =js></h4>
		<div class="row">
			<div class="col-xs-6">
				<address style="text-align: left;">
					<strong>Transaction Date:</strong>
					<js=row.transaction_date=js><br><br>
				</address>
				<address style="text-align: left;">
					<strong>Stakeholder Name:</strong><js=row.stakeholder_name=js><br>
					<strong>Address:</strong><js=row.stakeholder_address=js><br>
				</address>
			   
			</div>
			<div class="col-xs-6 text-right">
				Doc Type: <js=doc_type[row.document_type_const]=js><br>
				Doc No: <js=row.document_number=js><br>
			</div>
		</div>
		<div class="row">
			<div class="col-xs-12" style="float:left;text-align: left;">
				<b>Transaction Type:</b><js=row.transaction_type=js>
			</div>
		</div>
		<div class="row">
			<div class="col-md-12">
				<div class="panel panel-default">
					<div class="panel-heading">
						<h3 class="panel-title"><strong style="color:white!important">Transaction Details</strong></h3>
					</div>
					<div class="panel-body">
						<table class="table">
							<tbody>
								<tr>
									<th style="text-align:left;"><strong>ID</strong></th>
									<th class="text-center" style="text-align:left;"><strong>A/C</strong></th>
									<th class="text-center" style="text-align:left;"><strong>Explanation</strong></th>
									<th class="text-center" style="text-align:right;"><strong>Receive Amount</strong></th>
								</tr>
								<js::
									row.credit_amount = parseFloat( row.credit_amount );
									isNaN(row.credit_amount) ? row.credit_amount = 0 : undefined;
									row.credit_amount = row.credit_amount.toFixed(2);
									context.response.write(String.format(`<tr>
										<td style="text-align:left;">1</td>
										<td style="text-align:left;">{0}</td>
										<td style="text-align:left;">{1}</td>
										<td style="text-align:right;">{2}</td>
										</tr>`, row.ac_cd, row.explanation, row.credit_amount)
									);
									context.response.write(String.format(`<tr>
										<td style="text-align:right;" colspan=3>Total:</td>
										<td style="text-align:right;">{0}</td>
										</tr>`, row.credit_amount)
									);
									context.response.write(String.format('<tr><td style="text-align:left;font-size:14px;font-weight:bold;" colspan=3>In Word: {0}</td></tr>', currency.inWord( row.credit_amount )));
								::js>
								
							</tbody>
						</table>
					</div>
				</div>
			</div>
		</div>
	</div>
<js::}::js>
</impl-placeholder>
<impl-placeholder id="main_footer">
   
</impl-placeholder>