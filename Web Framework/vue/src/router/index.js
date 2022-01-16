import Vue from 'vue'
import VueRouter from 'vue-router'
import Login from '../components/Login.vue'
import Home from '../components/Home.vue'
import Welcome from '../components/Welcome.vue'
import Test from '../views/test/test.vue'
// import HoldingReturn from '../views/holding_return/index.vue'
// import QQPlot from '../views/qq_plot/index.vue'
// import QQPlotVol from '../views/qq_plot/vol.vue'
// import HVGVarious from '../views/hvg/various.vue'
// import HVGTimeSeries from '../views/hvg/time_series.vue'
// import HVGEval from '../views/hvg/eval.vue'
// // import MonthReturn from '../views/month_return/index.vue'
// // import IVSQuoteDay from '../views/ivs/quote_day.vue'
// import IVSSliceDay from '../views/ivs/slice_day.vue'
// import IVSTableDay from '../views/ivs/table_day.vue'
// import IVSSkewDay from '../views/ivs/skew.vue'
// import ScatterDailyRet from '../views/scatter/daily_ret.vue'
// import IVSScatterDay from '../views/ivs/ivs_scatter_day.vue'
// import IVSScatterIVS from '../views/scatter/ivs_scatter.vue'
// import IvsIntraday from '../views/ivs/ivs_intraday.vue'
// import FutureAndForwardBasis from '../views/basis/future_and_forward_basis.vue'
// // import FutureAndForwardTimeSeries from '../views/basis/future_and_forward_time_series.vue'
// import IvsSkewParams from '../views/ivs/skew_params.vue'
// import HVCone from '../views/volatility_cones/hv_cone.vue'
// import IVCone from '../views/volatility_cones/iv_cone.vue'

Vue.use(VueRouter)

const routes = [
]

const router = new VueRouter({
  routes: [
    { path: '/', redirect: '/login' },
    { path: '/login', component: Login },
    {
      path: '/home',
      component: Home,
      redirect: '/Welcome',
      children: [
        { path: '/home', component: Welcome },
        { path: '/test', component: Test },
        // { path: '/qq_plot', component: QQPlot },
        // { path: '/qq_plot_vol', component: QQPlotVol },
        // { path: '/hvg_various', component: HVGVarious },
        // { path: '/hvg_time_series', component: HVGTimeSeries },
        // { path: '/hvg_return_eval', component: HVGEval },
        // { path: '/month_return', component: MonthReturn },
        // { path: '/ivs_quote_day', component: IVSQuoteDay },
        // { path: '/ivs_quote_day_slice', component: IVSSliceDay },
        // { path: '/ivs_quote_day_table', component: IVSTableDay },
        // { path: '/ivs_skew_day', component: IVSSkewDay },
        // { path: '/scatter_daily_ret', component: ScatterDailyRet },
        // { path: '/ivs_scatter_day', component: IVSScatterDay },
        // { path: '/ivs_scatter_ivs', component: IVSScatterIVS },
        // { path: '/ivs_intraday', component: IvsIntraday },
        // { path: '/future_and_forward_basis', component: FutureAndForwardBasis },
        // { path: '/ivs_skew_params_day', component: IvsSkewParams },
        // { path: '/vol_surface_hv_cone', component: HVCone },
        // { path: '/vol_surface_iv_cone', component: IVCone },
      ]
    }
  ]
})

export default router
