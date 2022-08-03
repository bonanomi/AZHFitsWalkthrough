# AZH: Statistical analysis tour
## Structure of the datacards
### Naming scheme
The datacards follow the standard structure employed in [combine](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part2/settinguptheanalysis/).
More precisely, we tried to follow the naming conventions reported in the [HComb TWiki](https://twiki.cern.ch/twiki/bin/view/CMS/HiggsWG/HiggsCombinationConventions#Common_CMS_uncertainties) for what concerns the naming of the nuisance parameters. For what concerns the naming of the different processes, we tried to use self-explanatory names:
* `AtoZH`: for the signal
* `[DYJets, TT, TTZ, SingleTop, TTW, VV, WJets]`: list of backgrounds. `QCD` can also be included here, but in the case of FH channel, the yield is all the categories is completely negligible.

These are indicated in the cards under the `process` field. Note that there is a second `process` line with integers specified: 0 or negative integers define signal process(es), while positive integers define background process(es). More details on the structure of the datacards can be found [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part2/settinguptheanalysis/#a-simple-counting-experiment).

### Systematic uncertainties
We have implemented the complete set of systematic uncertainties, dividing them into rate uncertainties (i.e. described by a `lnN` p.d.f) and `shape` uncertainties. The former, e.g. `lumi`, `pdf`, and `QCD` scales from theory, are specified directly in the cards indicating their `%` effect on the rate, e.g.:

	lumi_13TeV_2017_uncorrelated                                                   lnN     1.02                  1.02                  1.02                  1.02

For a `2%` uncertainty on the normalizations coming from the systematic on luminosity.

The latter are implemented as `shape` uncertanties, for which the syntax is:

	CMS_trigger_sf_ee                                                              shape   1                     1                     1                     1

Where now the `1` indicates that histograms corresponding to `+/- 1sigma` variations of a specific uncertainty are found in the `.root` file associated to the datacard.

**Note:** When building the statistical model for `shape` uncertainties, `combine` will look for histograms with names `<PROCESS>_<NUISANCE>Up` and `<PROCESS>_<NUISANCE>Down` in the workspace. For example, for the `TT` process, you should have two histograms called `TT_CMS_trigger_sf_eeUp` and `TT_CMS_trigger_sf_eeDown` in the `.root` file.

The complete list of nuisance parameters included in the fit is documented in the AN, but it can also be inferred from reading a card (e.g. open `AZH_mA900_mH350_ZPT_diElectron_SignalRegion.dat`).

Last but not least: in all the cards you will see the last line with `* autoMCStats 10`. This implements for all the processes (`*` wildcard) the `autoMCStats`, i.e. the automatic bin-wise statistical uncertainties via the Barlow-Beeston-lite approach, as described [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part2/bin-wise-stats/). The parameter `10` referst to the threshold between Poisson- and Gaussian-constrained parameters, as described in the reference. This threshold may be changed in later optimizations of the analysis.

### Cards available
In this repository we make available cards for UL2017 for the `diMuon` and `diElectron` channels, for the following phase space regions:

* `SignalRegion`: SR cards with name `AZH_mA<X>_mH<Y>_<CHANNEL>_SignalRegion.dat`
* `CR0BTaggedJets`: cards for CR with 0-btagged jets with name `AZH_mA<X>_mH<Y>_<CHANNEL>_CR0BTaggedJets.dat`
* `CRZMassSidebands`: cards for sidebands CR with name `AZH_mA<X>_mH<Y>_<CHANNEL>_CRZMassSidebands.dat`

All the cards have an associated `.root` file that contains the histograms relevant for the fit, that is the histograms with the nominal (weighted) distributions for signal and background processes and the histograms for the `shape` uncertainties included in the fit. The latter follow the naming convention presented above, the former are named after the process they describe (e.g. if in the cards you see `AtoZH` you should expect to find a `TH1F` called `AtoZH` in the `.root` file).

These `.root` files have the same name as the datacards, except for the `.root` extension instead of `.dat`. For better handling and readibility of these files, we structured them using `TDirectoryFile` of `ROOT`, i.e. in the file `AZH_mA<X>_mH<Y>_<CHANNEL>_<REGION>.root` you will find a `TDirectoryFile` called `<REGION>` and therein all the relevant histograms for the fit.

## Running the fits
### Set up `combine`
To set up `combine` follow the [official instructions](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/#setting-up-the-environment-and-installation). For impact plots and pre/post-fit distributions we also need `CombineHarvester` that can be installed following [these instructions](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/#combine-tool).

### SR fits
We can start by running fits only in the SR. To do so, let's start by combining the `diElectron` and `diMuon` channel with `combineCards.py`:

	combineCards.py SignalRegion_diMuon=AZH_mA900_mH350_ZPT_diMuon_SignalRegion.dat SignalRegion_diElectron=AZH_mA900_mH350_ZPT_diElectron_SignalRegion.dat  > AZH_mA900_mH350_ZPT_SR.dat

You can now open `AZH_mA900_mH350_ZPT_SR.dat` to see how the combined card looks like. This output corresponds to the UL17 card combining `diElectron` and `diMuon` channel. When we have all three years available, we can run the same command for each year and then combine (again with `combineCards.py`) the cards for the three years into a single, full Run-II, card.

Some commands of combine require the creation of a `workspace`, i.e. a `.root` file where the statistical model is contained. To do so, we can use the `text2workspace.py` command on our `AZH_mA900_mH350_ZPT_SR.dat` card, as follows:

	text2workspace.py AZH_mA900_mH350_ZPT_SR.dat -m 125 -o AZH_mA900_mH350_ZPT_SR_ws.root

The option `-m 125` sets the mass of the Higgs boson in parametric models that depend on `mH`. In our case, this does not have any effect. The command above creates the `AZH_mA900_mH350_ZPT_SR_ws.root`, which contains in a `RooWorkspace` the information on the p.d.fs and nuisances included in the fit. It also applies a common signal strenght modifier `r` to the signal processes defined in the card.

#### FitDiagnostics

The first fit we can run uses `FitDiagnostics` (the most complete method of `combine`) to derive a measurement of the signal strength modifier:

	combine -M FitDiagnostics -m 125 --rMin -10 --rMax 10 -t -1 --expectSignal 1 --saveShapes --saveWithUncertainties -n _AZH_mA900_mH350_ZPT_SR_ws -d AZH_mA900_mH350_ZPT_SR_ws.root

A brief walkthrough in the options:
* `-n <OUTPUT_NAME>`: name of the output file. This will be called `fitDiagnostics_<OUTPUT_NAME>.root`
* `-d <INPUT_NAME>`: `FitDiagnostics` requires a `workspace` to run the fit and this is specified via the `-d` option, passing to it the `.root` file created with `text2workspace.py`
* `--rMin` and `--rMax`: these options define the range of the `r` (signal strenght) parameter.
* `-t -1`: tells `combine` to run a fit on an Asimov dataset (i.e. distributed according to MC with suppression of statistical fluctuations). More details on toys generation in `combine` can be found [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/runningthetool/#toy-data-generation).
* `--expectSignal 1`: the value of the signal strenght modifier to be used in the generation of the Asimov dataset. The general convention is `--expectSignal 1` for S+B fits and `--expectSignal 0` for B-only fits. However, it is also common to inject a signal different than 1 for S+B fits, depending on how `r` is defined.
* `--saveShapes --saveWithUncertainties`: these two options do not have an effect on the fit, but are used to save the pre/post-fit shapes and the uncertainty model for [plotting](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/nonstandard/#plotting) purposes.

#### PostFitShapes

The output of `FitDiagnostics` can also be used to create pre/post-fit plots of your distributions. To do so we need to run the `PostFitShapesFromWorkspace` command (this is a `CombineHarvester` command, so do not forget to install it):

	PostFitShapesFromWorkspace -d AZH_mA900_mH350_ZPT_SR.dat -w AZH_mA900_mH350_ZPT_SR_ws.root -o shapes_SR.root --sampling -m 125 --print -f fitDiagnostics_AZH_mA900_mH350_ZPT_SR_ws.root:fit_s --postfit

The options are akin to those used for `FitDiagnostics`, except that now we used `-d` to specify the datacard and `-w` to specify its associated workspace, while `-f` defines the output of `FitDiagnostics` used by `PostFitShapesFromWorkspace` to retrieve the distributions. More details on this command and its options can be found [here](https://cms-analysis.github.io/CombineHarvester/post-fit-shapes-ws.html).

We now have the `shapes_SR.root` file that contains all the inputs we need for plotting purposes. In this specific example we will have four `TDirectoryFile` named `SignalRegion_<CHANNEL>_prefit` and `SignalRegion_<CHANNEL>_postfit`, each containing the signal and background (both separated and inclusive) histograms.

The `plotDistributions.C` macro can be used to produce a plot of pre-fit (or post-fit) distributions and it can be launched as

	root -l plotDistribution.C

It should contain enough comments to be self-explanatory. Try repeating the steps above, starting from `FitDiagnostics`, changing the value of `--expectSignal <X>` to see how this impacts the resulting plots.

#### AsypmtoticLimits

In our analysis we want to set upper exclusion limits at 95% CL on the `xsec*BR` of the process under study. The test statistics is slightly different than the one used for the direct measurement of the cross section (or signal strength modifier) in `FitDiagnostics`. To achieve this goal we need to use `AsymptoticLimits` (more details [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/runningthetool/)):

	combine -M AsymptoticLimits -m 125 --run blind -d AZH_mA900_mH350_ZPT_SR.dat -n _AZH_mA900_mH350_ZPT_SR	

Here we use the `--run blind` option to run the fit blinded and to avoid using real data. The output looks like this:

	 -- AsymptoticLimits ( CLs ) --
	Expected  2.5%: r < 0.0179
	Expected 16.0%: r < 0.0244
	Expected 50.0%: r < 0.0347
	Expected 84.0%: r < 0.0521
	Expected 97.5%: r < 0.0758

and these points can be used to draw the classic "brazilian plot", where the central line will be the 50% quantile (`0.0347`) above, while the 16% and 84% quantiles can be used to draw the `+/-1sigma` band and the 2.5% and 97.5% ones to draw the `+/-2sigma` band.

Of course, to draw a meaningful brazilian plot we would need to run `AsymptoticLimits` for different `mA` (`mH`) values at a given `mH` (`mA`), thus the importance of having a large number of signal samples at the same `mA` (or `mH`) mass.

### SR+CR fits
All the commands above can be repeated for a fit where the SR and CR are combined. The only difference is the initial setup of the card. For example, if we want to run a simultaneous fit on SR and sidebands CR we can set up the card as follows:

	combineCards.py SignalRegion_diMuon=AZH_mA900_mH350_ZPT_diMuon_SignalRegion.dat SignalRegion_diElectron=AZH_mA900_mH350_ZPT_diElectron_SignalRegion.dat CRZMassSidebands_diMuon=AZH_mA900_mH350_ZPT_diMuon_CRZMassSidebands.dat CRZMassSidebands_diElectron=AZH_mA900_mH350_ZPT_diElectron_CRZMassSidebands.dat > AZH_mA900_mH350_ZPT_CRZMassSidebands.dat 

in a similar fashion, if we want to setup a fit where we include both CRs, we can extend the command above to include also the `AZH_mA900_mH350_ZPT_<CHANNEL>_CR0BTaggedJets.dat` cards.

**Note:** when running `PostFitShapesFromWorkspace` your output will contain a larger set of `TDirectoryFile`. Namely, one per each `<CHANNEL>_<REGION>` combination that entered the fit. This allows you to produce plots of pre/post-fit distributions for each of them.

**Note:** the current setup is suboptimal, as the CR files and cards do not contain contribution from signals. This needs to be fixed, as we also want to consider the (hopefully) marginal signal contributions present in the CRs.

#### Float background normalizations

When creating the SR+CR card as described in the previous section and running the fits using the commands defined above, we are running a fit where the SR and CR background normalizations are fixed to their expectation from MC and are not tied together. It is common practice to include a simultaneous scaling of the normalization of a given background process in SR and CR. In `combine` this can be done via `rateParam`, as described [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part5/longexercise/#a-use-of-rateparams) and [here](https://indico.cern.ch/event/577649/contributions/2339440/attachments/1380196/2097805/beyond_simple_datacards.pdf).

For example, given that the sidebands CR is largely populated by `TT` events we may want to exploit this to get a more reliable estimate of the normalization of this background and/or a constraint on some uncertainties. To do so, we can open the `AZH_mA900_mH350_ZPT_CRZMassSidebands.dat` card created above and add at the bottom:

	rate_TT rateParam SignalRegion_* TT 1
	rate_TT rateParam CRZMassSidebands_* TT 1

Where we are defining a `rate_TT` parameter that scales (`rateParam`) in the SR (`SignalRegion_*`) and CR (`CRZMassSidebands_*`) for all the channels (`*` wildcard) the `TT` process normalization. Essentially, this is equivalent to have a "signal strength modifier" on the `TT` process.

**Note:** when including `rateParam` that affect the normalization of a certain background process, we should make sure to "turn off" the theoretical uncertainties associated to that particular process.

After running the fit with `FitDiagnostics` you will be able to see the constraint on the normalization of this process by opening the `fitDiagnostics_<OUTPUT_NAME>.root` file and doing

	fit_b->Print() # for b-only fit
	fit_s->Print() # for s+b fit

### Impact and pulls
One important information to assess the soundness of the analysis and to inspect possible constraints on the nuisance parameters is the impact and pulls plot. This can be obtained using the `combineTool.py` script (again, from `CombineHarvester`) and its usage is straightforward, as described [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/nonstandard/#nuisance-parameter-impacts).