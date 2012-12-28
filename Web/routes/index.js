/*
 * GET home page.
 */

exports.index = function(req, res){
	var data = { 
		title: 'GroMon Dashboard'
		, farenheight: '79'
		, celcius: '26'
		, humidity: '93'
	};

	res.render('index', data);
};
