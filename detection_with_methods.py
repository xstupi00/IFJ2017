import perun.check as check
import perun.postprocess.regression_analysis.tools as tools
import perun.postprocess.regression_analysis.regression_models as regression_models
import perun.profile.query as query
import perun.logic.runner as runner

import copy, math

__author__ = 'Simon Stupinsky'

ignore_var = 0


def get_best_models_of(profile, model_type=None, actual_uid=None):
    if model_type is None:
        best_model_map = {
            uid: ("", 0.0, 0.0, 0.0, 0.0) for uid in query.unique_model_values_of(profile, 'uid')
        }
        for _, model in query.all_models_of(profile):
            model_uid = model['uid']
            if best_model_map[model_uid][1] < model['r_square']:
                if model['model'] == 'logarithmic':
                    profile = refactor_log_profile(profile)
                if model['model'] == 'quadratic':
                    best_model_map[model_uid] = (
                        model['model'], model['r_square'], model['coeffs'][0]['value'],
                        model['coeffs'][1]['value'], model['coeffs'][2]['value'])
                else:
                    best_model_map[model_uid] = (
                        model['model'], model['r_square'], model['coeffs'][0]['value'], model['coeffs'][1]['value'])
    else:
        best_model_map = {
            actual_uid: ("", 0.0, 0.0, 0.0, 0.0)
        }
        for _, model in query.all_models_of(profile):
            if model['uid'] == actual_uid and model['model'] == model_type:
                best_model_map[actual_uid] = (
                    model['model'], model['r_square'], model['coeffs'][0]['value'], model['coeffs'][1]['value'])

    return best_model_map, profile


def get_value(model, formula, coordinates_x):
    try:
        formula_fx = regression_models._MODELS[model[1][0]]['f_x']
        coordinates_x = formula_fx(coordinates_x)
    except KeyError:
        pass
    # formula_fa = regression_models._MODELS[model[1][0]]['f_a']
    # formula_fb = regression_models._MODELS[model[1][0]]['f_b']

    # model[1][2] = formula_fa(model[1][2])
    # model[1][3] = formula_fb(model[1][3])

    if model[1][0] == 'quadratic':
        value = formula(model[1][2], model[1][3], model[1][4], coordinates_x)
    else:
        value = formula(model[1][2], model[1][3], coordinates_x)

    # formula_fy = regression_models._MODELS[model[1][0]]['f_y']
    # value = formula_fy(formula_fy)

    return value


def check_const_err(baseline_model, target_model, std_err_profile, count_var, list_y):
    stud_res_profile = copy.deepcopy(std_err_profile)
    del stud_res_profile['global']['models']

    average_y = sum(list_y) / float(len(list_y))

    lvg = counter = index = sum_mse = 0
    for y in list_y:
        lvg += (y - average_y) ** 2
    length = len(list_y)
    list_y.reverse()

    relevant_constant = target_model[1][2] - baseline_model[1][2]
    for resources in query.all_resources_of(std_err_profile):
        for std_err in query.all_key_values_of(resources[1], 'amount'):
            if resources[1]['uid'] == baseline_model[0]:
                if 0.90 * relevant_constant <= std_err <= 1.10 * relevant_constant:
                    counter += 1
            list_item = list_y.pop()
            try:
                leverage = (((list_item - average_y) ** 2) / lvg) + (1 / length)
            except ZeroDivisionError:
                leverage = (((list_item - average_y) ** 2) / tools.APPROX_ZERO) + (1 / length)
            index += 1
            sum_mse += std_err ** 2
            mse = sum_mse / index
            try:
                stud_res_profile['global']['resources'][index - 1]['amount'] = std_err / (
                    math.sqrt(mse) * math.sqrt(1 - leverage))
            except ZeroDivisionError:
                stud_res_profile['global']['resources'][index - 1]['amount'] = 0.0

    if (0.90 * count_var <= counter <= 1.10 * count_var):
        return True

    if (0.90 * relevant_constant <= std_err_profile['global']['models'][5]['coeffs'][0][
        'value'] <= 1.10 * relevant_constant):
        return True

    stud_res_profile = runner.run_postprocessor_on_profile(stud_res_profile, 'regression_analysis', {
        "regression_models": [],
        "steps": 3,
        "method": "full"
    }, True)
    if 0.85 <= stud_res_profile['global']['models'][5]['coeffs'][0]['value'] <= 1.15:
        return True

    return False


def check_linear_err(baseline_profile, target_profile, std_err_profile, avg_rel_err, uid):
    baseline_linear_models, baseline_profile = get_best_models_of(baseline_profile, 'linear', uid)
    target_linear_models, target_profile = get_best_models_of(target_profile, 'linear', uid)

    for baseline_model, target_model in zip(baseline_linear_models.items(), target_linear_models.items()):
        if baseline_model[0] == uid:
            baseline_b1 = baseline_model[1][3]
            request_diff = avg_rel_err * baseline_b1
            if (0.85 * (baseline_b1 + request_diff) <= target_model[1][3] <= 1.15 * (
                        baseline_b1 + request_diff)):
                return True

    return False


def check_quad_err(std_err_profile, uid):
    best_error_models, std_err_profile = get_best_models_of(std_err_profile, 'quadratic', uid)

    for error_model in best_error_models.items():
        if error_model[0] == uid and error_model[1][1] > 0.95:
            return True

    return False


def refactor_log_profile(profile):
    index = 0
    end = False
    for resources in query.all_resources_of(profile):
        for coordinates_y in query.all_key_values_of(resources[1], 'amount'):
            if coordinates_y < 0:
                index += 1
            elif coordinates_y >= 0:
                end = True
                break
        if end:
            break

    global ignore_var
    ignore_var = index

    while index:
        del profile['global']['resources'][index - 1]
        index -= 1

    profile = runner.run_postprocessor_on_profile(profile, 'regression_analysis', {
        "regression_models": [],
        "steps": 3,
        "method": "full"
    }, True)

    return profile


def detection_with_methods(baseline_profile, target_profile):
    global ignore_var

    best_baseline_models, baseline_profile = get_best_models_of(baseline_profile)
    best_target_models, target_profile = get_best_models_of(target_profile)

    for baseline_model, target_model in zip(best_baseline_models.items(), best_target_models.items()):
        if target_model[1][0] == 'logarithmic' and ignore_var:
            while ignore_var:
                del baseline_profile['global']['resources'][ignore_var - 1]
                ignore_var -= 1

        std_err_profile = copy.deepcopy(baseline_profile)
        del std_err_profile['global']['models']
        div_mod_profile = copy.deepcopy(std_err_profile)
        rel_err_profile = copy.deepcopy(std_err_profile)
        baseline_formula = regression_models._MODELS[baseline_model[1][0]
        ]['transformations']['plot_model']['formula']

        target_formula = regression_models._MODELS[target_model[1][0]
        ]['transformations']['plot_model']['formula']

        THRESHOLD_B0 = abs(0.05 * baseline_model[1][2])
        THRESHOLD_B1 = abs(0.05 * baseline_model[1][3])
        sum_rel_err = sum_abs_err = index = count_var = 0
        change = None
        change_type = ''
        if (abs(target_model[1][2] - baseline_model[1][2]) <= THRESHOLD_B0
            and abs(target_model[1][3] - baseline_model[1][3]) <= THRESHOLD_B1):
            change = check.PerformanceChange.NoChange

        list_y = list()
        for resources in query.all_resources_of(baseline_profile):
            for coordinates_x in query.all_key_values_of(resources[1], 'structure-unit-size'):
                for coordinates_y in query.all_key_values_of(resources[1], 'amount'):
                    if resources[1]['uid'] == baseline_model[0]:
                        f_b = get_value(
                            baseline_model, baseline_formula, coordinates_x)
                        f_t = get_value(
                            target_model, target_formula, coordinates_x)
                        std_err_profile['global']['resources'][index]['amount'] = f_t - f_b
                        try:
                            div_mod_profile['global']['resources'][index]['amount'] = f_t / f_b
                        except ZeroDivisionError:
                            div_mod_profile['global']['resources'][index]['amount'] = f_t / tools.APPROX_ZERO
                        try:
                            rel_err_profile['global']['resources'][index]['amount'] = (f_t - f_b) / f_b
                        except ZeroDivisionError:
                            rel_err_profile['global']['resources'][index]['amount'] = (f_t - f_b) / tools.APPROX_ZERO
                        sum_abs_err += std_err_profile['global']['resources'][index]['amount']
                        sum_rel_err += rel_err_profile['global']['resources'][index]['amount']
                        list_y.append(coordinates_y)
                        count_var += 1
            index += 1
        for _ in range(count_var, index):
            del std_err_profile['global']['resources'][count_var]
            del div_mod_profile['global']['resources'][count_var]
            del rel_err_profile['global']['resources'][count_var]

        sum_rel_err = sum_rel_err / count_var * 100

        std_err_profile = runner.run_postprocessor_on_profile(std_err_profile, 'regression_analysis', {
            "regression_models": [],
            "steps": 3,
            "method": "full"
        }, True)
        std_err_constant_model, std_err_profile = get_best_models_of(std_err_profile, 'constant', baseline_model[0])
        if std_err_constant_model[baseline_model[0]][1] >= 0.95 \
                and -0.10 <= std_err_constant_model[baseline_model[0]][2] <= 0.10:
            change = check.PerformanceChange.NoChange

        div_mod_profile = runner.run_postprocessor_on_profile(div_mod_profile, 'regression_analysis', {
            "regression_models": [],
            "steps": 3,
            "method": "full"
        }, True)
        div_mod_constant_model, div_mod_profile = get_best_models_of(div_mod_profile, 'constant', baseline_model[0])
        if div_mod_constant_model[baseline_model[0]][1] >= 0.95 \
                and 0.90 <= div_mod_constant_model[baseline_model[0]][2] <= 1.10:
            change = check.PerformanceChange.NoChange

        rel_err_profile = runner.run_postprocessor_on_profile(rel_err_profile, 'regression_analysis', {
            "regression_models": [],
            "steps": 3,
            "method": "full"
        }, True)

        if baseline_model[1][0] == target_model[1][0] and change is None:
            change = check.PerformanceChange.Degradation
            if check_const_err(baseline_model, target_model, std_err_profile, count_var, list_y):
                change_type = 'constant '
            elif (
                    check_linear_err(baseline_profile, target_profile, std_err_profile, sum_rel_err / 100,
                                     baseline_model[0])):
                change_type = 'linear '
            elif check_quad_err(std_err_profile, baseline_model[0]):
                change_type = 'quadratic '
            else:
                change_type = 'unidentified '
            if sum_abs_err > 0:
                change_type += 'error'
            else:
                change = check.PerformanceChange.Optimization
                change_type += 'improvement'
        elif change is None:
            if sum_abs_err > 0:
                change = check.PerformanceChange.Degradation
                change_type = 'unidentified error'
            else:
                change = check.PerformanceChange.Optimization
                change_type += 'unidentified improvement'

        best_corresponding_baseline_model = best_baseline_models.get(
            baseline_model[0])
        if best_corresponding_baseline_model:
            confidence = min(
                best_corresponding_baseline_model[1], target_model[1][1])

        yield check.DegradationInfo(
            change, change_type, baseline_model[0],
            best_corresponding_baseline_model[0],
            target_model[1][0],
            sum_rel_err,
            'r_square', confidence
        )
